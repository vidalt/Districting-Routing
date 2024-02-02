class District:

    def __init__(self, district_id, block_id_list, avg_tsp_cost, average_num_costumers,
                 max_x, min_x, max_y, min_y, area, min_depot_dist, average_depot_dist):
        
        self.district_id = district_id
        self.block_id_list = block_id_list
        self.avg_tsp_cost = avg_tsp_cost
        self.average_num_costumers = average_num_costumers
        
        self.max_x = max_x
        self.min_x = min_x
        self.max_y = max_y
        self.min_y = min_y
        
        self.height = abs(max_y - min_y)
        self.width  = abs(max_x - min_x)
        
        self.rectangle_area_without_depot = self.height * self.width
        
        # S is defined as the ratio of the length of the service region to the height (or vice versa) so that S/> 1
        self.s = max(self.height/self.width, self.width/self.height)
        
        self.area = area
        self.min_depot_dist = min_depot_dist
        self.average_depot_dist = average_depot_dist
        
        
    def calculate_metrics_with_depot(self, depot_position):
        considered_height = self.height
        considered_width = self.width
        
        if depot_position['x'] < self.min_x:
            considered_width  = abs(self.max_x - depot_position['x'])
        
        elif depot_position['x'] > self.max_x:
            considered_width  = abs(depot_position['x'] - self.min_x)
        
        if depot_position['y'] < self.min_y:
            considered_height  = abs(self.max_y - depot_position['y'])
        
        elif depot_position['y'] > self.max_y:
            considered_height  = abs(depot_position['y'] - self.min_y)
        
        self.rectangle_area_with_depot = considered_height * considered_width
        
        

    def __str__(self):

        s = "[%d] Avg Cost %f %s Average Customers %d Value of S %f Area %f \n RectDepArea %f RectNoDepArea %f" % \
        (self.district_id, self.avg_tsp_cost, self.block_id_list, self.average_num_costumers, self.s, self.area, \
         self.rectangle_area_with_depot, self.rectangle_area_without_depot)
        return s
