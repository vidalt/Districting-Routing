# Dataset Generation

This folder contains the code to randomly build feasible districts and get their expected cost using the Sampling Approximation Approach (SAA). It is done by solving TSP with Lin–Kernighan algorithm considering training scenarios.

To compile the C++ code just run ``make`` command, it is necessary to have compiler g++ installed.
To execute it:

```
./districts <city_name> <target_district_size> <number_of_generated_districs> <depot_position> <number_of_bus> <random_seed>
```

Example:

```
./districts Bristol 30 10000 C 120 0
```

This code expects to have city data in ``../Dataset/<city_name>.geojson`` and customer scenarions in ``../Dataset/scenarios/<city_name>_<depot_position>_<number_of_bus>_<target_district_size>.train.json``.


The output file is in json format and it is expected to have the following properties:

```json
{
    "districts": [
        {
	    "average-cost": "double",
	    "list-blocks": ["int"],
	    "scenarios-cost": ["double"],
	    "scenarios-nbCustomers": ["int"]
	}
    ], 
    
    
    "metadata": {         
	 "TIME_TO_GENERATE": "float"
    }
}
```

## Our Generated Data

You can find our generated TSP data [here]([https://www.example.com](https://www.dropbox.com/scl/fo/cklionmqz1wd6z1c9jw64/h/tsp-data?dl=0&subfolder_nav_tracking=1})).
Our repository is designed to have this data on the folder: 

