import os
import json
import pandas as pd
from os import listdir
from os.path import isfile, join
import numpy as np
import math

df = pd.DataFrame(columns = ['InstanceName'])
script_dir = os.path.dirname(__file__)
rel_path = "./models"
other_cost_estimators = ['SNN', 'BD', 'FIG']
other_cost_estimator_rel_path = '../CostEstimators/models'

abs_file_path = os.path.join(script_dir, rel_path)
instances = [f for f in listdir(abs_file_path) if not isfile(join(abs_file_path, f)) ]

for instance in instances:
    instance_size = int(instance.split('_')[2])
    target_district_size = int(instance.split('_')[3])
    depot_positioning = instance.split('_')[1]
    
    if  instance_size > 120:
        continue

    # if instance_size != 90 or target_district_size != 12:
    #     continue
    

    costs = {}
    
    for estimator in other_cost_estimators:
        try:
            file = open(f"{other_cost_estimator_rel_path}/{estimator}/{instance}/training-data.json" , "r")
            json_data = json.load(file)
            costs[estimator] = float(json_data['exec-time'])
        except FileNotFoundError:
            print(f"{estimator}/{instance}")

    file = open(abs_file_path + "/" + instance + "/training-data.json", "r")
    jsonData =json.load(file)

    try:
    
        data = {'InstanceName' : instance, 
                'GNN': float(jsonData['execution-time']),
                'BD':  costs['BD'],
                'SNN': costs['SNN'],
                'FIG': costs['FIG']
        }
    except Exception:
        print(f"Solution {instance} with size {instance_size} and target {target_district_size} with Depot {depot_positioning} deu pau")


    df = df.append(data, ignore_index=True)

df['City'] = [ instance.split('_')[0] for instance in df['InstanceName']]
df['Depot'] = [ instance.split('_')[1] for instance in df['InstanceName']]
df['Central Depot'] = [ instance.split('_')[1] == "C" for instance in df['InstanceName']]
df['Size'] = [ int(instance.split('_')[2]) for instance in df['InstanceName']]
df['Target Size District'] = [ int(instance.split('_')[3]) for instance in df['InstanceName']]

df = df.sort_values(by=['Size', 'Target Size District', 'Central Depot', 'City', 'Depot'])

# df = df.sort_values(by=['Central Depot'])

df.drop(['InstanceName'], axis='columns', inplace=True)

df_grouped = df.groupby(['Size', 'Target Size District'])['GNN', 'BD', 'SNN', 'FIG'].mean()
# df_grouped = df.groupby(['Central Depot', 'City'])['GNN', 'BD', 'SNN', 'FIG'].mean()


print(df_grouped.head())
df_grouped.to_csv('exec-times.csv', sep='\t')

