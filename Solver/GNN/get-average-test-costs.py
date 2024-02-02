import os
import json
import pandas as pd
from os import listdir
from os.path import isfile, join
import numpy as np
import math

df = pd.DataFrame(columns = ['InstanceName'])
script_dir = os.path.dirname(__file__)
rel_path = "../../Data/TSP-Scenarios/districts-with-costs"

abs_file_path = os.path.join(script_dir, rel_path)
instances = [f for f in listdir(abs_file_path) if isfile(join(abs_file_path, f)) and "train_and_test.json" in f]

total = len(instances)
it = 0
for instance in instances:
    
    print(f"Runned {it}/{total}")
    it += 1
    
    instance_size = int(instance.split('_')[2])
    target_district_size = int(instance.split('_')[3])
    depot_positioning = instance.split('_')[1]
    
    # if  instance_size > 120:
    #     continue

    if instance_size != 90 or target_district_size != 12:
        continue
    
    # print(instance.split('_'))

    costs = {}
    
    file = open(abs_file_path + "/" + instance, "r")
    jsonData =json.load(file)
    
    every_cost = np.array([district['average-cost'] for district in jsonData['districts'] if 'TEST_SET' in district])
    average_cost = np.mean(every_cost)
    
    every_conf = np.array([district['confidence-interval-range'] for district in jsonData['districts'] if 'TEST_SET' in district])
    average_conf = np.mean(every_conf)
    
    try:
    
        data = {'InstanceName' : instance, 
                'SAA_test': average_cost,
                'Confidence_Interval': average_conf
        }
    except Exception:
        print(f"Solution {instance} with size {instance_size} and target {target_district_size} with Depot {depot_positioning} deu pau")

    df = df.append(data, ignore_index=True)

df['City'] = [ instance.split('_')[0] for instance in df['InstanceName']]
df['Depot'] = [ instance.split('_')[1] for instance in df['InstanceName']]
df['Central Depot'] = [ instance.split('_')[1] == "C" for instance in df['InstanceName']]
df['Size'] = [ int(instance.split('_')[2]) for instance in df['InstanceName']]
df['Target Size District'] = [ int(instance.split('_')[3]) for instance in df['InstanceName']]

df = df.sort_values(by=['Central Depot'])

# df_grouped = df.groupby(['Size', 'Target Size District'])['SAA_test', 'Confidence_Interval'].mean()
df_grouped = df.groupby(['Central Depot', 'City'])['SAA_test', 'Confidence_Interval'].mean()


print(df_grouped.head())
df_grouped.to_csv('test_costs.csv', sep='\t')