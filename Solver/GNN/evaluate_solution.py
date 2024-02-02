from solution_evaluator import SolutionEvaluator

import argparse
import os
import sys

os.environ['KMP_DUPLICATE_LIB_OK'] = 'True'

def parse_arguments():
    parser = argparse.ArgumentParser()

    parser.add_argument('-seed', type=int, default=1)
    parser.add_argument('-n_epoch', type=int, default=10000, help='number of epochs')
    parser.add_argument('-learning_rate', type=float, default=0.0001)
    parser.add_argument('-mode', default='cpu', help='cpu/gpu')
    parser.add_argument('-gm', default='mean_field', help='mean_field/loopy_bp')
    parser.add_argument('-data', default=None, help='data folder name')
    parser.add_argument('-batch_size', type=int, default=64, help='minibatch size')
    parser.add_argument('-feat_dim', type=int, default=0, help='dimension of node feature')
    parser.add_argument('-num_class', type=int, default=0, help='#classes')
    parser.add_argument('-fold', type=int, default=1, help='fold (1..10)')

    parser.add_argument('-latent_dim', type=int, default=64, help='dimension of latent layers')
    parser.add_argument('-out_dim', type=int, default=1024, help='s2v output size')
    parser.add_argument('-hidden', type=int, default=100, help='dimension of regression')
    parser.add_argument('-max_lv', type=int, default=4, help='max rounds of message passing')

    parser.add_argument('-plot_training', type=int, default=1)
    parser.add_argument('-training', type=int, default=0)

    parser.add_argument('-solution_file', type=str, default="none")
    parser.add_argument('-data_file', type=str, default="empty")
    parser.add_argument('-data_type', type=str, default="json")

    return parser.parse_args()


if __name__ == '__main__':

    args = parse_arguments()
    # print("***********************************************************")
    sys.stdout.flush()

    evaluator = SolutionEvaluator(args)
    evaluator.get_cost_from_solution()
