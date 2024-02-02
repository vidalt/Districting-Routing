import argparse
from models.shallow_neural_network import SNN
from models.figliozi_continuous_formula import FIG
from models.bd_formula import BD

parser = argparse.ArgumentParser()
parser.add_argument('-instance_name', help='Instace Name')
args = parser.parse_args()

instance_name = args.instance_name

snn = SNN(args.instance_name, use_test_set=True)
snn.run_test_evaluation()

# fig = FIG(args.instance_name, test_set=True)
# bd = BD(args.instance_name, test_set=True)
