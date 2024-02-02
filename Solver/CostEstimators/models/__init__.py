from cgi import test
import geojson
import pandas as pd
import numpy as np
import random
import pickle
import os
import time
import json

from abc import ABC, abstractmethod
from domain.district import District
from domain.block import Block
from domain.city import City
from sklearn.metrics import mean_squared_error
from sklearn.metrics import mean_absolute_percentage_error
from utils import calculate_distance

class BaseModel(ABC):

    def __init__(self, instance_name, regressor, model_name, load_predicting_mode=False, use_test_set=False):
        self.regressor  = regressor
        self.model_name = model_name
        self.instance_name = instance_name
        self.load_predicting_mode = load_predicting_mode
        self.use_test_set = use_test_set
        self.calculate_city_by_instance(instance_name, instance_name.split('_')[0])
        
        TRAINING_SET_PERCENTAGE = 0.8
        VALIDATION_SET_PERCENTAGE = 0.1
        TEST_SET_PERCENTAGE = 0.1

        random.seed(0)
        random.shuffle(self.city.district_list)

        pos1 = int(TRAINING_SET_PERCENTAGE * len(self.city.district_list))
        pos2 = int(TEST_SET_PERCENTAGE * len(self.city.district_list))

        self.training_set = self.city.district_list[:pos1]
        self.test_set = self.city.district_list[pos1:pos1+pos2]
        self.validation_set = self.city.district_list[pos1+pos2:]
                
        
    def calculate_city_by_instance(self, instance_name, city_name):
        
        self.instance_name = instance_name
    
        with open(f"../../Data/Dataset/{city_name}.geojson") as city_file:
            city_data = geojson.load(city_file)

        type_of_scenario = "test" if self.use_test_set else "train"
        
        with open(f"../../Data/Dataset/scenarios/{instance_name}.{type_of_scenario}.json") as instance_file:    
            instance_data = geojson.load(instance_file)

        city_features = city_data['features']
        blocks_properties_df = pd.DataFrame([city_feature['properties'] for city_feature in city_features])

        depot_longlat = instance_data['metadata']['DEPOT_LONGLAT']
        depot_xy = {'x': instance_data['metadata']['DEPOT_XY'][0], 'y': instance_data['metadata']['DEPOT_XY'][1]}
        depot_xy_list = [depot_xy['x'], depot_xy['y']]
        proba_customer_demand = instance_data['metadata']['PROBA_CUSTOMER_DEMAND']

        self.depot_xy = depot_xy
        blocks = []

        for block in instance_data['blocks']:
            block_id = block['ID']
            dist_depot = block['DEPOT_DIST']
            average_customers_depot_dist = block['AVG_DEPOT_DIST']
            customers_weight             = block['TOTAL_CUSTOMERS']
            
            block_properties= blocks_properties_df[blocks_properties_df['ID'] == block_id]
            area = block_properties['AREA']
            adjacent_block_id_list = block_properties['LIST_ADJACENT']
            population = block_properties['POPULATION']
            density = block_properties['DENSITY']
            perimeter = block_properties['PERIMETER']

            block_points = block_properties['POINTS']

            max_x = np.max([block_point[0] for block_point in block_points])
            min_x = np.min([block_point[0] for block_point in block_points])
            max_y = np.max([block_point[1] for block_point in block_points])
            min_y = np.min([block_point[1] for block_point in block_points])

            customers = block['SCENARIOS']
            
            average_customers = block['AVG_NUM_CUSTOMERS']
            current_block = Block(block_id, adjacent_block_id_list, population, area,\
                                density, perimeter, dist_depot, max_x, min_x, max_y, min_y, customers, \
                                average_customers_depot_dist, customers_weight, average_customers)

            blocks.append(current_block)
        
        self.blocks = blocks
        
        districts = []
        
        if self.load_predicting_mode is False:
            with open(f"../../Data/TSP-Scenarios/districts-with-costs/{instance_name}_tsp.train_and_test.json") as scenarios_file:    
                scenarios_data = geojson.load(scenarios_file)

            district_id = 0

            for district_data in scenarios_data['districts']:
               
                cost = district_data['average-cost']
                list_adj = district_data['list-blocks']    
                
                blocks_district = [block for block in blocks if block.block_id in list_adj]

                max_x = np.max([block.max_x for block in blocks_district])
                min_x = np.min([block.min_x for block in blocks_district])
                max_y = np.max([block.max_y for block in blocks_district])
                min_y = np.min([block.min_y for block in blocks_district])
                
                average_customers = np.sum([block.average_customers for block in blocks_district])

                total_customers = np.sum([block.total_customers for block in blocks_district])
                sum_depot_dist  = np.sum([block.total_customers*block.avg_depot_dist for block in blocks_district])
                
                average_depot_distance = sum_depot_dist/total_customers
        
                total_area = np.sum([block.area for block in blocks_district])
                min_depot_dist = np.min([block.dist_depot for block in blocks_district])

                district = District(district_id, list_adj, cost, average_customers,\
                                    max_x, min_x, max_y, min_y, total_area, min_depot_dist,\
                                    average_depot_distance)

                district.calculate_metrics_with_depot(depot_xy)

                districts.append(district)
                district_id += 1


        self.city = City(city_name, blocks, districts)
    

    
    def get_features_and_labels_from_dataset(self, dataset):
        X = [self.get_features_from_district(district) for district in dataset]
        y = [district.avg_tsp_cost for district in dataset]
        return (X, y)
    
    @abstractmethod
    def get_features_from_district(self, district: District):
        pass
    
    def run_training(self):
        
        X_train, y_train = self.get_features_and_labels_from_dataset(self.training_set)
        
        initial_time = time.perf_counter()
        self.regressor = self.regressor.fit(X_train, y_train)
        elapsed_time = time.perf_counter() - initial_time

        X_test, y_test = self.get_features_and_labels_from_dataset(self.test_set)
        y_pred = self.regressor.predict(X_test)
        
        mse, mae = mean_squared_error(y_test, y_pred), mean_absolute_percentage_error(y_test, y_pred)
        
        training_data = json.loads(json.dumps({
            'mse-test-set': mse,
            'mae-test-set': mae,
            'exec-time': elapsed_time
        }))
        
        self.save_model(training_data)

        return mse, mae
    
    def run_test_evaluation(self):
        self.load_model()
        
        X_test, y_test = self.get_features_and_labels_from_dataset(self.test_set)
        y_pred = self.regressor.predict(X_test)
        
        mse, mae = mean_squared_error(y_test, y_pred), mean_absolute_percentage_error(y_test, y_pred)
        
        folder = f"./models/{self.model_name}/{self.instance_name}/"
        current_test_data = json.load(open(f"{folder}training-data.json", 'r'))
        
        print(f"current_test_data {current_test_data}")
        
        current_test_data['mse-test-set'] = mse
        current_test_data['mae-test-set'] = mae
                
        print(f"current_test_data {current_test_data}")
        json.dump(current_test_data, open(f"{folder}training-data.json", 'w'))

        
    
    def save_model(self, training_data):
        folder = f"./models/{self.model_name}/{self.instance_name}/"
        os.makedirs(os.path.dirname(folder), exist_ok=True)
        
        pickle.dump(self.regressor, open(f"{folder}best.model", 'wb'))
        json.dump(training_data, open(f"{folder}training-data.json", 'w'))
        
    def load_model(self):
        self.regressor = pickle.load(open(f"./models/{self.model_name}/{self.instance_name}/best.model", 'rb'))
        print(self.regressor)
        
    def evaluate(self, districts):
        
        dataset = []
        district_id = 0
               
        for list_adj in districts:
            
            blocks_district = [block for block in self.city.block_list if block.block_id in list_adj]
            max_x = np.max([block.max_x for block in blocks_district])
            min_x = np.min([block.min_x for block in blocks_district])
            max_y = np.max([block.max_y for block in blocks_district])
            min_y = np.min([block.min_y for block in blocks_district])

            average_customers = np.sum([block.average_customers for block in blocks_district]) + 1
            
            total_customers = np.sum([block.total_customers for block in blocks_district])
            sum_depot_dist  = np.sum([block.total_customers*block.avg_depot_dist for block in blocks_district])
            average_depot_distance = sum_depot_dist/total_customers
            
            total_area = np.sum([block.area for block in blocks_district])
            min_depot_dist = np.min([block.dist_depot for block in blocks_district])

            district = District(district_id, list_adj, -1, average_customers,\
                                max_x, min_x, max_y, min_y, total_area, min_depot_dist, average_depot_distance)

            district_id+=1
            district.calculate_metrics_with_depot(self.depot_xy)
            
            dataset.append(district)
        
        X = [self.get_features_from_district(district) for district in dataset]
        result = self.regressor.predict(X)
        
        return result
    
    def get_validation_set_metrics(self):
        X = [self.get_features_from_district(district) for district in self.validation_set]
        result = self.regressor.predict(X)
        
        return result

