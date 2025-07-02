import math
import geojson
import pandas as pd
import numpy as np
import random

from k_means_constrained import KMeansConstrained
from domain.district import District
from domain.block import Block
from domain.city import City

from collections import deque


class Scenarios():

    def __init__(self, instance_name):
        self.instance_name = instance_name
        self.load_prediction_mode = False
        
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
        
        # self.calculate_test_set_metrics()
              
              
    def calculate_test_set_metrics(self):
        counter = 0
        for district in self.test_set:
            blocks_district = [block for block in self.blocks if block.block_id in district.block_id_list]
            blocks_scenarios = np.array([block.scenarios for block in blocks_district])
                
            depot_array = np.array(self.depot_xy_list)
            all_points = np.concatenate([np.concatenate(scenario) for scenario in blocks_scenarios if len(scenario) > 0])

            points_array = np.array(all_points)

            distances = np.linalg.norm(points_array - depot_array, axis=1)
            customer_depot_distance_std = np.std(distances)
            customer_depot_distance_min = np.min(distances)
            
            
            baricenter = np.mean(points_array, axis=0)
            vector_depot_baricenter = baricenter - self.depot_xy_list

            u = vector_depot_baricenter / np.linalg.norm(vector_depot_baricenter)
            v = [-u[1], u[0]]
            
            orthogonal_customer_pos = np.dot(points_array, v)
            orthogonal_customer_pos_std = np.std(orthogonal_customer_pos)
            
            district.customer_depot_distance_min = customer_depot_distance_min
            district.customer_depot_distance_std = customer_depot_distance_std
            district.orthogonal_customer_pos_std = orthogonal_customer_pos_std
            
            district.calculate_compactness()
            
            if counter % 50 == 0: print(counter)
            counter += 1
        
    def calculate_city_by_instance(self, instance_name, city_name):
    
        self.instance_name = instance_name
        self.instance_size = int(instance_name.split("_")[2])
        self.target_district_size = int(instance_name.split("_")[3])
    
        with open(f"../../Data/Dataset/{city_name}.geojson") as city_file:
            city_data = geojson.load(city_file)

        type_of_scenario = "train"
        
        with open(f"../../Data/Dataset/scenarios/{instance_name}.{type_of_scenario}.json") as instance_file:    
            instance_data = geojson.load(instance_file)

        city_features = city_data['features']
        blocks_properties_df = pd.DataFrame([city_feature['properties'] for city_feature in city_features])

        depot_longlat = instance_data['metadata']['DEPOT_LONGLAT']
        depot_xy = {'x': instance_data['metadata']['DEPOT_XY'][0], 'y': instance_data['metadata']['DEPOT_XY'][1]}
        self.depot_xy_list = [depot_xy['x'], depot_xy['y']]
        proba_customer_demand = instance_data['metadata']['PROBA_CUSTOMER_DEMAND']
        
        self.depot_xy = depot_xy
        blocks = []

        for block in instance_data['blocks']:
            block_id = block['ID']
            dist_depot = block['DEPOT_DIST']
            max_dist_depot = block['MAX_DEPOT_DIST']
            average_customers_depot_dist = block['AVG_DEPOT_DIST']
            total_customers             = block['TOTAL_CUSTOMERS']
            
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

            blocks_scenarios = block['SCENARIOS']
            
            customers = [block_scenario for block_scenario in blocks_scenarios if len(block_scenario) > 0]
            
            all_customer = np.concatenate([np.concatenate(customers)])
            barycenter = np.mean(all_customer, axis=0)
            
            average_customers = block['AVG_NUM_CUSTOMERS']
            
            current_block = Block(block_id, adjacent_block_id_list, population, area,\
                                density, perimeter, dist_depot, max_x, min_x, max_y, min_y, customers, \
                                average_customers_depot_dist, total_customers, average_customers, max_dist_depot, block_points)
            
            current_block.barycenter = barycenter
                        
            blocks.append(current_block)
        
        self.blocks = blocks
        
        districts = []
        
        if True:
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
                max_depot_dist = np.max([block.max_depot_dist for block in blocks_district])
                
                all_points = []
                
                # for block in blocks_district:
                #     all_points = all_points + block.all_points.tolist()[0]

                district = District(district_id, list_adj, cost, average_customers,\
                                    max_x, min_x, max_y, min_y, total_area, min_depot_dist,\
                                    average_depot_distance, max_depot_dist, all_points)

                # district.calculate_metrics_with_depot(depot_xy)
                # district.calculate_min_angle(depot_xy)
                
                districts.append(district)
                district_id += 1


        self.city = City(city_name, blocks, districts)
    
    def export(self):
        districts_id = [district.district_id for district in self.test_set]
        blocks_list = [district.block_id_list for district in self.test_set]
        
        min_depot_dist = [district.min_depot_dist for district in self.test_set]
        max_depot_dist = [district.max_depot_dist for district in self.test_set]

        std_customer_distance = [district.customer_depot_distance_std for district in self.test_set]
        avg_customer_distance = [district.average_depot_dist for district in self.test_set]
        customer_depot_distance_min = [district.customer_depot_distance_min for district in self.test_set]
        orthogonal_customer_pos_std = [district.orthogonal_customer_pos_std for district in self.test_set]
        
        compactness = [district.compactness_measure for district in self.test_set]

        true_cost = [district.avg_tsp_cost for district in self.test_set]
        
        df = pd.DataFrame({
            "id": districts_id,
            "block_list": blocks_list,
            # "min_depot_dist": min_depot_dist,
            "max_depot_dist": max_depot_dist,
            "saa_cost": true_cost,
            "std_customer_distance": std_customer_distance,
            "avg_customer_distance": avg_customer_distance,
            "min_customer_distance": customer_depot_distance_min,
            "std_ort_customer_postion": orthogonal_customer_pos_std,
            "compactness": compactness
        })
        
        df.to_csv(f"./results/{self.instance_name}-test-set-data.csv")

    def solve_balanced_k_means(self):
        
        barycenters = [block.barycenter for block in self.blocks]
        number_of_clusters = self.instance_size / self.target_district_size
        
        min_size = math.floor(self.target_district_size * 0.8)
        max_size = math.ceil(self.target_district_size * 1.2)

        clf = KMeansConstrained(
            n_clusters=int(number_of_clusters),
            size_min=min_size,
            size_max=max_size,
            random_state=0
        )
        
        clf.fit_predict(barycenters)

        labels = (clf.labels_)
        
        unique_classes = np.unique(labels)
        num_classes = len(unique_classes)

        matrix = [[] for _ in range(num_classes)]

        for index, class_label in enumerate(labels):
            matrix[class_label].append(index)

        districts = [np.array(indices, dtype=int) for indices in matrix]
        
        any_non_contigous = False
        
        for district_blocks in districts:
            size_district = len(district_blocks)
            
            if size_district > max_size:
                print("Wrong max Size")
            if size_district < min_size:
                print("Wrong min size")
            
            if not self.is_district_contigous(district_blocks):
                any_non_contigous = True
                print("Non Contiguous")
                break
        
        if not any_non_contigous:
            print("ALL RIGHT")
            
        content = [
            self.instance_name,
            "feasible" if not any_non_contigous else "infeasible",
            "NUMBER ITER 0",
            "COST -1",
            "EXECUTION TIME -1"
        ]

        # Add the numeric data lists to the content
        for data_list in districts:
            content.append('\t'.join(map(str, data_list)))

        with open(f"./solutions/{self.instance_name}", 'w') as file:
            for line in content:
                file.write(line + '\n')
            
        
        
    def is_district_contigous(self, blocks_district):
                    
        # Sets for units to visit and units in the district
        units_to_visit = set(blocks_district)
        units_of_district = set(blocks_district)       
        
        # Initialize the queue with the initial unit
        next_unit_to_visit = deque([blocks_district[0]])
        units_to_visit.remove(blocks_district[0])
        
        while next_unit_to_visit:
            current_unit = next_unit_to_visit.popleft()
            
            # Process neighbors
            neighbors = np.array(self.blocks[current_unit].adjacent_block_id_list)[0]
            for neighbor in neighbors:
                
                if neighbor in units_of_district and neighbor in units_to_visit:
                
                    units_to_visit.remove(neighbor)
                    next_unit_to_visit.append(neighbor)
        
        return not units_to_visit