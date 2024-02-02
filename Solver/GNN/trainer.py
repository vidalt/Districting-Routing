import matplotlib as mpl

mpl.use('Agg')
import matplotlib.pyplot as plt

import random, math, time, sys

import sys
import os
import torch
import random
import numpy as np
from tqdm import tqdm
from torch.autograd import Variable
from torch.nn.parameter import Parameter
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import time
import json
import networkx as nx

from collections import deque
from cost_estimators.models.shallow_neural_network import SNN

from s2v_lib.embedding import EmbedMeanField, EmbedLoopyBP
from s2v_lib.mlp import MLPRegression
from s2v_lib.pytorch_util import *
import matplotlib.ticker as plticker

from data_model import *

TRAINING_SET_PERCENTAGE = 0.8
VALIDATION_SET_PERCENTAGE = 0.1
TEST_SET_PERCENTAGE = 0.1


class Brain(nn.Module):

    def __init__(self, args, num_node_feats, num_edge_feats):
        super(Brain, self).__init__()

        self.mode = args.mode
        self.num_node_feats = num_node_feats
        self.num_edge_feats = num_edge_feats

        if args.gm == 'mean_field':
            model = EmbedMeanField
        elif args.gm == 'loopy_bp':
            model = EmbedLoopyBP
        else:
            print('unknown gm %s' % args.gm)
            sys.exit()
        self.s2v = model(latent_dim=args.latent_dim,
                         output_dim=args.out_dim,
                         num_node_feats=self.num_node_feats,
                         num_edge_feats=self.num_edge_feats,
                         max_lv=args.max_lv)
        self.mlp = MLPRegression(input_size=args.out_dim, hidden_size=args.hidden)

    def forward(self, batch_graph, node_feat, edge_feat, label=None):

        if self.mode == 'gpu':
            node_feat = node_feat.cuda()
            edge_feat = edge_feat.cuda()

            if label is not None:
                label = label.cuda()

        embed = self.s2v(batch_graph, node_feat, edge_feat)

        if label is None:
            return self.mlp(embed)
        else:
            return self.mlp(embed, label)

class S2VGraph(object):
    def __init__(self, g):

        self.num_nodes = g.number_of_nodes()
        x, y, w = zip(*g.edges(data=True))
        self.num_edges = len(x)
        self.edge_pairs = np.ndarray(shape=(self.num_edges, 2), dtype=np.int32)
        self.edge_pairs[:, 0] = x
        self.edge_pairs[:, 1] = y
        self.edge_pairs = self.edge_pairs.flatten()


        self.edge_weight = np.ndarray(shape=(self.num_edges, 1), dtype=np.int32)
        self.edge_weight[:, 0] = [x["weight"] for x in w]

#TODO: Distance in edges
#TODO: More training data

class Trainer:

    def __init__(self, args):

        random.seed(0)

        self.args = args
        self.num_node_feats = 8 # Population and isOnDistrict
        self.num_edge_feats = 1

        self.brain = Brain(args, self.num_node_feats, self.num_edge_feats)
        self.optimizer = optim.Adam(self.brain.parameters(), lr=args.learning_rate)

        if args.mode == 'gpu':
            self.brain = self.brain.cuda()

        self.instance_name = args.city_name + "_" + args.depot_position + "_" + str(args.instance_size) + "_" + str(args.target_size_district)

        if args.data_type == "json":
            data_loader = DataLoaderJSON(self.instance_name, evaluating=self.args.training == 0)
        
        elif args.data_type == "vanilla":
            data_loader = DataLoader("../Datasets/%s" % args.data_file)
        else:
            raise Exception("Data Type unknown")

        self.city = data_loader.city
        random.shuffle(self.city.district_list)

        pos1 = int(TRAINING_SET_PERCENTAGE * len(self.city.district_list))
        pos2 = int(TEST_SET_PERCENTAGE * len(self.city.district_list))
        
        self.training_set = self.city.district_list[:pos1]
        self.test_set = self.city.district_list[pos1:pos1+pos2]
        self.validation_set = self.city.district_list[pos1+pos2:]

        self.patience_threshold = 1000



    def make_nn_input(self, instance_batch):

        graph_batch = []
        node_feat = []
        edge_feat = []
        label_list = []

        g = S2VGraph(self.city.city_graph)
        pop_list = [block.population / self.city.max_population for block in self.city.block_list]
        perimeter_list = [block.perimeter / self.city.max_perimeter for block in self.city.block_list]
        area_list = [block.area / self.city.max_area for block in self.city.block_list]
        density_list = [block.density / self.city.max_density for block in self.city.block_list]
        dist_depot_list = [block.dist_depot / self.city.max_dist_depot for block in self.city.block_list]
            
        for i, district in enumerate(instance_batch):
            in_district_list = [1 if block.block_id in district.block_id_list else 0 for block in self.city.block_list]
            zipped_feat = [list(x) for x in zip(pop_list, np.sqrt(pop_list), perimeter_list, area_list, np.sqrt(area_list), density_list, dist_depot_list, in_district_list)]

            graph_batch.append(g)
            cur_node_feat = [zipped_feat[i] for i in range(g.num_nodes)]
            node_feat.append(cur_node_feat) 

            cur_edge_feat = np.repeat(g.edge_weight, 2, axis=0)

            edge_feat.append(cur_edge_feat)

            label_list.append(district.avg_tsp_cost)

        node_feat_tensor = torch.FloatTensor(node_feat).reshape(g.num_nodes * len(instance_batch), self.num_node_feats) # size (NODE X FEATURE)
        edge_feat_tensor = torch.FloatTensor(edge_feat).reshape(g.num_edges * 2 * len(instance_batch), self.num_edge_feats)  # size (2*EDGE X FEATURE)
        label_tensor = torch.FloatTensor(label_list).reshape(len(instance_batch), 1)

        return graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor

    def run_learner(self):

        train_idxes = list(range(len(self.training_set)))
        test_idxes = list(range(len(self.validation_set)))

        if self.args.plot_training:
            training_loss_record = []
            test_loss_record = []
            epoch_record = []

        best_loss = None
        patience = 0

        initial_time = time.perf_counter()

        for epoch in range(self.args.n_epoch):
            random.shuffle(train_idxes)

            test_loss = self.loop_dataset(self.validation_set, test_idxes)
            print('\033[93maverage test of epoch %d: loss %.5f acc %.5f\033[0m' % (epoch, test_loss[0], test_loss[1]))

            train_loss = self.loop_dataset(self.training_set, train_idxes)
            print('\033[92maverage training of epoch %d: loss %.5f acc %.5f\033[0m' % (epoch, train_loss[0], train_loss[1]))

            if best_loss is None or test_loss[0] < best_loss:
                best_loss = test_loss[0]
                print('----saving to best model since this is the best valid loss so far.----')
                
                os.makedirs(os.path.dirname('./models/%s/epoch-best.model' % self.instance_name), exist_ok=True)
                torch.save(self.brain.state_dict(), './models/%s/epoch-best.model' % self.instance_name)
                #save_args(cmd_args.save_dir + '/epoch-best-args.pkl', cmd_args)

            if self.args.plot_training:

                training_loss_record.append(train_loss[0])
                test_loss_record.append(test_loss[0])
                epoch_record.append(epoch)
                plt.clf()

                axes = plt.gca()
                axes.set_ylim([0, 50])

                plt.plot(epoch_record, training_loss_record, label="training loss", linestyle="-", color='y')
                plt.plot(epoch_record, test_loss_record, label="testing loss", linestyle="--", color='b')
                plt.xlabel('# Epoch')
                plt.ylabel('Loss (MSE)')
                plt.legend()
                out_file = 'log_training.png'
                plt.savefig('./models/%s/%s' % (self.instance_name, out_file))

            if best_loss is None or test_loss[0] < best_loss:
                best_loss = test_loss[0]
                patience = 0

            elif patience == self.patience_threshold:
                break
            else:
                patience += 1

        elapsed_time = time.perf_counter() - initial_time
        output_json = {"execution-time": elapsed_time,
                       "best-training-loss": best_loss,
                       "executed-for-epochs": epoch}

        with open('./models/%s/training-data.json' % self.instance_name, 'w', encoding='utf-8') as f:
            json.dump(output_json, f, ensure_ascii=False, indent=4)

    def loop_dataset(self, training_set, sample_idxes):
        total_loss = []
        total_iters = (len(sample_idxes) + (self.args.batch_size - 1) * (self.optimizer is None)) // self.args.batch_size
        pbar = tqdm(range(total_iters), unit='batch')

        n_samples = 0
        for pos in pbar:
            selected_idx = sample_idxes[pos * self.args.batch_size: (pos + 1) * self.args.batch_size]

            instance_batch = [training_set[idx] for idx in selected_idx]

            graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor = self.make_nn_input(instance_batch)

            _, loss, acc = self.brain(graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor)

            if self.optimizer is not None:
                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()

            loss = to_scalar(loss)
            pbar.set_description('loss: %0.5f acc: %0.5f' % (loss, acc))

            total_loss.append(np.array([loss, acc.cpu().detach().numpy()]) * len(selected_idx))

            n_samples += len(selected_idx)

        if self.optimizer is None:
            assert n_samples == len(sample_idxes)
        total_loss = np.array(total_loss)
        avg_loss = np.sum(total_loss, 0) / n_samples
        return avg_loss

    def run_evaluater(self):

        probabilities_of_customer_demand =  {
            3:  0.004,
            6:  0.002, 
            12: 0.001, 
            20: 0.0006, 
            30: 0.0004
        }

        current_prob_customer = probabilities_of_customer_demand[self.args.target_size_district]

        model_file = './models/%s/epoch-best.model' % self.instance_name
    
        self.brain.load_state_dict(torch.load(model_file, map_location='cpu'), strict=True)

        graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor = self.make_nn_input(self.test_set)
        
        preds, mae, mse = self.brain(graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor)

        preds = preds.data.cpu().numpy().flatten()
        true_y = label_tensor.data.cpu().numpy().flatten()

        result_file = open('./models/%s/training-data.json' % self.instance_name, "r")
        jsonData =json.load(result_file)
        result_file.close()

        initial_time = time.perf_counter()

        elapsed_time = time.perf_counter() - initial_time

        result_file = open('./models/%s/training-data.json' % self.instance_name, "w")

        jsonData['validation-loss'] = mse.data.item()
        
        json.dump(jsonData, result_file)
        result_file.close()
        
        # snn = SNN(self.instance_name)
        # snn.load_model()
        # snn_preds = snn.get_validation_set_metrics()
        
        # lower_bound_tsp = [(district.avg_tsp_cost - district.confidence_interval) for district in self.test_set]
        # upper_bound_tsp = [(district.avg_tsp_cost + district.confidence_interval) for district in self.test_set]
        
        # true_y, lower_bound_tsp, upper_bound_tsp, snn_preds, preds = zip(*sorted(zip(true_y, lower_bound_tsp, upper_bound_tsp, snn_preds, preds)))
        
        # mpl.rc('font',**{'family':'sans-serif','sans-serif':['Helvetica'], 'size': '16'})
        # plt.clf()

        # axes = plt.gca()
        
        # min_length = 100
        # max_length = 400

        # plt.plot(range(max_length - min_length), snn_preds[min_length:max_length], label="SNN Prediction", linestyle="-", color="#440154", linewidth=1., zorder= 1)
        # plt.plot(range(max_length - min_length), preds[min_length:max_length], label="GNN Prediction", linestyle="-", color="#56c667", linewidth=1., zorder=2)
        # plt.plot(range(max_length - min_length), true_y[min_length:max_length], label="True Routing Cost", linestyle="-", color="#fde725", linewidth=1., alpha=0.55, zorder=0)
        
        # # plt.plot(range(len(self.test_set)), upper_bound_tsp, label="True Routing Cost", linestyle="--", color="r", linewidth=1.2)        
        # plt.fill_between(range(max_length - min_length), lower_bound_tsp[min_length:max_length], upper_bound_tsp[min_length:max_length], color="#fde725", alpha=0.85, zorder=10)

        
        # plt.xlabel('Districts', fontsize=20)
        
        # plt.tick_params( axis='x',         
        #                  which='both',      
        #                  bottom=False,      
        #                  top=False,         
        #                  labelbottom=False)
        
        # loc = plticker.MultipleLocator(base=5.0) # this locator puts ticks at regular intervals
        # axes.yaxis.set_major_locator(loc)
        
        # plt.ylabel('Routing Cost', fontsize=20)
        # plt.legend(fontsize=15, loc='lower right')
        # plt.savefig('./models/%s/test-comparison.png' % self.instance_name)

        # plt.savefig('./models/%s/test-comparison.pdf' % self.instance_name, format='pdf', bbox_inches='tight', pad_inches=0.1)

    def preload_city_data(self):
        self.g = S2VGraph(self.city.city_graph)
        self.pop_list = [block.population / self.city.max_population for block in self.city.block_list]
        self.perimeter_list = [block.perimeter / self.city.max_perimeter for block in self.city.block_list]
        self.area_list = [block.area / self.city.max_area for block in self.city.block_list]
        self.density_list = [block.density / self.city.max_density for block in self.city.block_list]
        self.dist_depot_list = [block.dist_depot / self.city.max_dist_depot for block in self.city.block_list]
        model_file = "./models/%s/epoch-best.model" % (self.args.data_file.split('.geojson')[0])
        self.brain.load_state_dict(torch.load(model_file, map_location='cpu'), strict=True)

    def make_nn_input_from_preloaded_data(self, instance_batch):
        graph_batch = []
        node_feat = []
        edge_feat = []
        label_list = []

        for i, district in enumerate(instance_batch):
            in_district_list = [1 if block.block_id in district.block_id_list else 0 for block in self.city.block_list]
            zipped_feat = [list(x) for x in zip(self.pop_list, np.sqrt(self.pop_list), self.perimeter_list, self.area_list, np.sqrt(self.area_list), self.density_list, self.dist_depot_list, in_district_list)]

            graph_batch.append(self.g)
            cur_node_feat = [zipped_feat[i] for i in range(self.g.num_nodes)]
            node_feat.append(cur_node_feat)

            cur_edge_feat = np.repeat(self.g.edge_weight, 2, axis=0)

            edge_feat.append(cur_edge_feat)

            label_list.append(district.avg_tsp_cost)

        node_feat_tensor = torch.FloatTensor(node_feat).reshape(self.g.num_nodes * len(instance_batch), self.num_node_feats) # size (NODE X FEATURE)
        edge_feat_tensor = torch.FloatTensor(edge_feat).reshape(self.g.num_edges * 2 * len(instance_batch), self.num_edge_feats)  # size (2*EDGE X FEATURE)
        label_tensor = torch.FloatTensor(label_list).reshape(len(instance_batch), 1)

        return graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor
    
    def get_cost_from_pre_loaded(self, district_list):
        graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor = self.make_nn_input_from_preloaded_data(district_list)
        preds, _, _ = self.brain(graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor)
        preds = preds.data.cpu().numpy().flatten()

        return preds