import geojson
import math
import numpy as np
from models import BaseModel
from domain.district import District

class simple_regressor():
    def __init__(self):
        self.beta = float('inf')
    
    def predict(self, districts):        
        return self.predict_by_beta(districts, self.beta)

    
    def predict_by_beta(self, districts, beta):
        costs = [(2 * district['avg_depot_distance'] \
                + (beta/100) * math.sqrt(district['total_area'] * district['expected_number_of_clients'])) \
                for district in districts]
        return costs
    
    def fit(self, X_train, y_train):
        best_mse = float('inf')
        best_beta = -1
        
        for beta in range(1000):
            cur_result = self.predict_by_beta(X_train, beta=beta)
            cur_mse = ((np.array(y_train) - np.array(cur_result)) ** 2).mean(0)
            
            if cur_mse < best_mse:
                best_beta = beta
                best_mse = cur_mse
            
        self.beta = best_beta
        return self
        
    
    
class BD(BaseModel):
    
    def __init__(self, instance_name, regressor=None, model_name='BD', load_predicting_mode=False, use_test_set=False):   
        regressor = simple_regressor()        
        super(BD, self).__init__(instance_name, regressor, model_name, load_predicting_mode, use_test_set)     
        
    
    def get_features_from_district(self, district: District):
        return {
            'total_area': district.area,
            'expected_number_of_clients': district.average_num_costumers,
            'avg_depot_distance': district.average_depot_dist,
        }
