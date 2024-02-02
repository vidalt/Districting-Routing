class Block:

    def __init__(self, block_id, adjacent_block_id_list, population, area, density, perimeter, \
                 dist_depot, max_x, min_x, max_y, min_y, scenarios, avg_depot_dist, total_customers, \
                 average_customers):
        
        self.block_id = block_id
        self.adjacent_block_id_list = adjacent_block_id_list
        self.population = population
        self.area = area
        self.density = density
        self.perimeter = perimeter
        self.dist_depot = dist_depot
        
        self.max_x = max_x
        self.min_x = min_x
        self.max_y = max_y
        self.min_y = min_y
        
        self.scenarios = scenarios
        self.avg_depot_dist = avg_depot_dist
        self.total_customers = total_customers
        self.average_customers = average_customers

    def __str__(self):

        s = "[%d] %d %f %f %f %f %s Square [%f %f % f %f]" % (self.block_id, self.population, self.area,
                                           self.density, self.perimeter, self.dist_depot,
                                           self.adjacent_block_id_list, 
                                           self.max_x, self.min_x, self.max_y, self.min_y)
        return s