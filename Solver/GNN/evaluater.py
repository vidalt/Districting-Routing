

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

import networkx as nx

from collections import deque

from s2v_lib.embedding import EmbedMeanField, EmbedLoopyBP
from s2v_lib.mlp import MLPRegression
from s2v_lib.pytorch_util import *

from data_model import *

from trainer import Brain

class Evaluater:

    def __init__(self, args):
        self.args = args

        self.model = Brain(args)

        model_file = "../results-paris/epoch-best.model"

        self.model.load_state_dict(torch.load(model_file, map_location='cpu'), strict=True)

        data_loader = DataLoader("../Datasets/Paris.dat")

        self.city = data_loader.city
        print(self.city)
        random.shuffle(self.city.district_list)

        self.test_set = self.city.district_list[:100]
        graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor = self.make_nn_input(instance_batch)

        _, loss, acc = self.brain(graph_batch, node_feat_tensor, edge_feat_tensor, label_tensor)

        print(loss)