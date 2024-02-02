import os
import pandas as pd
from os import listdir
from os.path import isfile, join

script_dir = os.path.dirname(__file__)
rel_path = "Solutions/eval"

abs_file_path = os.path.join(script_dir, rel_path)
files = [f for f in listdir(abs_file_path) if isfile(join(abs_file_path, f)) and f.endswith('bd.eval')  
         and '120' in f]

df = pd.DataFrame(columns = ['Instance Name', 'Neural Net Cost', 'Beardwood Raganzo Cost'])


for fileName in files:
    file_bd = open(abs_file_path + "/" + fileName, "r")
    cost_bd = float(file_bd.readline().split(" ")[-1])
    
    instance_name = fileName.split(".")[0]

    file_nn = open(abs_file_path + "/" + instance_name + ".nn.eval", 'r')
    cost_nn = float(file_nn.readline().split(" ")[-1])

    data = {'Instance Name': instance_name, 
            'Neural Net Cost': cost_nn,
            'Beardwood Raganzo Cost' : cost_bd}

    df = df.append(data, ignore_index=True)

df = df.sort_values('Instance Name')
df.to_csv("tmp.csv", sep='\t')

