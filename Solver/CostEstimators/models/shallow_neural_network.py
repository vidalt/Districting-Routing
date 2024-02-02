import math
from models import BaseModel

from domain.district import District
from sklearn.neural_network import MLPRegressor


class SNN(BaseModel):
    
  
    def __init__(self, instance_name, regressor=None, model_name='SNN', load_predicting_mode=False, use_test_set=False):
        regressor  = regressor if regressor != None else \
                  MLPRegressor(random_state=0, max_iter=50000, hidden_layer_sizes=(3,))
                          
        super(SNN, self).__init__(instance_name, regressor, model_name, load_predicting_mode, use_test_set)        

    def get_features_from_district(self, district: District):
        return [district.average_num_costumers, district.s, district.s/district.average_num_costumers, \
                district.average_depot_dist, math.sqrt(district.average_num_costumers*district.area)]