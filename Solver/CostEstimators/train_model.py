import argparse
from models.shallow_neural_network import SNN
from models.figliozi_continuous_formula import FIG
from models.bd_formula import BD

parser = argparse.ArgumentParser()
parser.add_argument('-instance_name', help='Instace Name')
args = parser.parse_args()

instance_name = args.instance_name

snn = SNN(args.instance_name)
snn.run_training()

fig = FIG(args.instance_name)
fig.run_training()

bd = BD(args.instance_name)
bd.run_training()

