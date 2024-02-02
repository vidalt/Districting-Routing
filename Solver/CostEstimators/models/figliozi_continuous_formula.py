import math
from models import BaseModel

from domain.district import District
from sklearn.linear_model import LinearRegression


class FIG(BaseModel):
    
  
    def __init__(self, instance_name, regressor=None, model_name='FIG', load_predicting_mode=False, use_test_set=False):
        regressor  = LinearRegression()
        super(FIG, self).__init__(instance_name, regressor, model_name, load_predicting_mode, use_test_set)        

    def get_features_from_district(self, district: District):
        return [(district.average_num_costumers-1)*math.sqrt(district.area/district.average_num_costumers) ,\
                math.sqrt(district.area/district.average_num_costumers), district.average_depot_dist]