from trainer import Trainer
from  data_model import District

import numpy as np
import json


class SolutionEvaluator:

    def __init__(self, instance_file="empty"):
        args = type('', (), {})()

        args.mode = 'cpu'
        args.seed=1
        args.n_epoch=10000
        args.learning_rate=0.0001

        args.mode='cpu'
        args.gm='mean_field'
        args.data=None

        args.batch_size=64
        args.feat_dim=0
        args.num_class=0
        args.fold=1
        args.latent_dim=64
        args.out_dim=1024
        args.hidden=100
        args.max_lv=4
        args.plot_training=1
        args.training=0
        
        args.data_file=instance_file
        args.data_type="json"
        
        instance_data = instance_file.split('_')

        args.city_name = instance_data[0]
        args.depot_position = instance_data[1]
        args.instance_size = instance_data[2]
        args.target_size_district = instance_data[3].split('.')[0]

        self.args = args
        self.trainer = Trainer(args)
        self.trainer.preload_city_data()

    def evaluate(self, solution):
        districts = [District(district_id, block_list, 0) for district_id, block_list in enumerate(solution)]
        preditec_costs = self.trainer.get_cost_from_pre_loaded(districts)
        costs = ";".join(map(str, preditec_costs.tolist()))        
        return costs

    def get_cost_from_solution(self):

        with open("../ILS/data/%s" % self.args.solution_file, 'r') as file:

            solution_json = json.load(file)
            districts = [District(district_id, block_list, 0) for district_id, block_list in enumerate(solution_json['districts'])]

        preditec_costs = self.trainer.get_cost(districts)
        total_cost = np.sum(preditec_costs)

        solution_json['estimated_cost'] = float(total_cost.real)

        with open("../ILS/data/%s" % self.args.solution_file, 'w') as file:
            json.dump(solution_json, file)


        
