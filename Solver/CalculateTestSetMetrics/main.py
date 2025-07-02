import argparse

from scenarios import Scenarios

parser = argparse.ArgumentParser()
parser.add_argument('-instance_name', help='Instace Name')
args = parser.parse_args()

instance_name = args.instance_name
dist_scenarios = Scenarios(instance_name)

dist_scenarios.solve_balanced_k_means()