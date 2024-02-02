import networkx as nx
import numpy as np
import geojson

class City:

    def __init__(self, city_name, block_list, district_list):
        self.city_name = city_name
        self.block_list = block_list
        self.district_list = district_list
        self.city_graph = self.build_graph()

        self.max_tsp_cost = max(self.district_list, key=lambda x: x.avg_tsp_cost).avg_tsp_cost
        self.max_population = max(self.block_list, key=lambda x: x.population).population
        self.max_perimeter = max(self.block_list, key=lambda x: x.perimeter).perimeter
        self.max_density = max(self.block_list, key=lambda x: x.density).density
        self.max_area = max(self.block_list, key=lambda x: x.area).area
        self.max_dist_depot = max(self.block_list, key=lambda x: x.dist_depot).dist_depot



    def build_graph(self):
        g = nx.Graph()

        for b1 in self.block_list:
            for b2 in b1.adjacent_block_id_list:
                g.add_edge(b1.block_id, b2, weight=1)

        return g



    def __str__(self):

        s = "[%s]\n" % self.city_name

        s += "[BLOCKS]\n"

        for b in self.block_list:
            s += str(b) + "\n"

        s += "[DISTRICTS]\n"

        for d in self.district_list[:10]:
            s += str(d) + "\n"
        s += "[...]\n"
        for d in self.district_list[-10:]:
            s += str(d) + "\n"
        return s


class Block:

    def __init__(self, block_id, adjacent_block_id_list, population, area, density, perimeter, dist_depot):
        self.block_id = block_id
        self.adjacent_block_id_list = adjacent_block_id_list
        self.population = population
        self.area = area
        self.density = density
        self.perimeter = perimeter
        self.dist_depot = dist_depot

    def __str__(self):

        s = "[%d] %d %f %f %f %f %s" % (self.block_id, self.population, self.area,
                                           self.density, self.perimeter, self.dist_depot,
                                           self.adjacent_block_id_list)
        return s



class District:

    def __init__(self, district_id, block_id_list, avg_tsp_cost, confidence_interval):
        self.district_id = district_id
        self.block_id_list = block_id_list
        self.avg_tsp_cost = avg_tsp_cost
        self.confidence_interval = confidence_interval

    def __str__(self):

        s = "[%d] %f %s" % (self.district_id, self.avg_tsp_cost, self.block_id_list)
        return s

class DataLoader:

    def __init__(self, city_file):

        g = nx.Graph()
        expected_nodes = -1
        cur_state = ""
        city_name = ""
        district_list = []
        block_list = []
        with open(city_file, 'r') as f:
            for i, line in enumerate(f):

                if i == 0:
                    ls = line.strip().split(' ')
                    city_name = ls[1]

                if line.strip() == "[VERTICES]":
                    cur_state = "VERTICES"
                elif line.strip() == "[BLOCKS]":
                    cur_state = "BLOCKS"
                elif line.strip() == "[DISTRICTS]":
                    cur_state = "DISTRICTS"
                elif line.strip() == "[BLOCK-TO-BLOCK-DISTANCES]":
                    cur_state = "BLOCK-TO-BLOCK-DISTANCES"
                elif line.strip() == "[BLOCK-FEATURES]":
                    cur_state = "BLOCK-FEATURES"

                elif cur_state == "BLOCKS":
                    ls = [int(x) for x in line.strip().split(' ')]
                    new_block = self.build_block(ls)
                    block_list.append(new_block)

                elif cur_state == "DISTRICTS":
                    ls = line.strip().split(' ')
                    new_district = self.build_district(ls)
                    district_list.append(new_district)

                elif cur_state == "BLOCK-FEATURES":
                    ls = [x for x in line.strip().split(' ')]
                    id = int(ls[0])
                    perimeter, area, density = [float(x) for x in ls[1:]]
                    cur_block = block_list[id]
                    assert(cur_block.block_id == id)
                    cur_block.add_features(perimeter, area, density)

        self.city = City(city_name, block_list, district_list)



        # r_g = nx.convert_node_labels_to_integers(g)

    def build_block(self, block_info_list):

        block_id = block_info_list[0] - 1
        n_vertex = block_info_list[1]
        vertex_list = block_info_list[2:2+n_vertex]
        n_inhabitant = block_info_list[2+n_vertex]
        n_adjacent_block = block_info_list[2+n_vertex+1]
        adjacent_block = [b - 1 for b in block_info_list[2+n_vertex+2:]]

        block = Block(block_id, n_inhabitant, adjacent_block)

        return block

    def build_district(self, district_info_list):

        district_id = int(district_info_list[0]) - 1
        n_block = int(district_info_list[1])
        block_list = [int(x) - 1 for x in district_info_list[2:2+n_block]]
        tsp_cost = float(district_info_list[2+n_block])

        district = District(district_id, block_list, tsp_cost)

        return district


class DataLoaderJSON:

    def __init__(self, instance_name, evaluating=False):
        g = nx.Graph()

        district_list = []
        block_list = []

        city_name = instance_name.split('_')[0]

        with open("../../Data/Dataset/%s.geojson" % city_name, 'r') as f:
            data = geojson.load(f)

        # Build blocks
        instance_size = int(instance_name.split('_')[2])

        blocks = data['features']

        for b in blocks:

            block_info = b['properties']
            if block_info["NAME"] != "DEPOT" and block_info['ID'] < instance_size:

                list_adj = [adj for adj in block_info['LIST_ADJACENT'] if adj < instance_size]

                new_block = Block(block_info['ID'],  list_adj, block_info['POPULATION'],
                                  block_info['AREA'], block_info['DENSITY'], block_info['PERIMETER'], 0)

                block_list.append(new_block)

        type_of_scenario = 'train'

        with open("../../Data/Dataset/scenarios/%s.%s.json" % (instance_name, type_of_scenario), 'r') as f:
            scenarios_data = geojson.load(f)

        for block_id, block_data in enumerate(block_list):
            block_list[block_id].dist_depot = scenarios_data['blocks'][block_id]['DEPOT_DIST']
            

        with open("../../Data/TSP-Scenarios/districts-with-costs/%s_tsp.train_and_test.json" % instance_name, 'r') as f:
             tsp_data = geojson.load(f)

        districts = tsp_data['districts']

        for i, district_info in enumerate(districts):

            new_district = District(i, district_info["list-blocks"], district_info["average-cost"], district_info["confidence-interval-range"])
            district_list.append(new_district)

        self.city = City(city_name, block_list, district_list)


