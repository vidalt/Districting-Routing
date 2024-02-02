# Dataset generation

This folder contains the code to build each city geojson file and the customers scenarios files.

To compile the C++ code just run ``make`` command, it is necessary to have compiler g++ installed.
To execute it:

```
./generate_scenarios <city_name> <reference_longitude> <reference_latitude> <scenarios_per_block> <random_seed>
```

Example:

```
./generate_scenarios Bristol -2.58791 51.454514 50 0
```


## Building City Data

Five metropolitan areas in the United Kingdoms are considered: Bristol, Manchester, Leeds, London, and West-Midlands. In order to have the data about the cities, census data, provided by [Park (2018)](https://www.ons.gov.uk/peoplepopulationandcommunity/populationandmigration/populationestimates/datasets/middlesuperoutputareamidyearpopulationestimates), and polygon definition for each zone, available on [Uber (2021)](https://movement.uber.com/?lang=en-CA), were joined using a zone id, each zone defines
a basic unit.

The coordinates of each BU have been projected to cartesian coordinates using the center of the city as reference point and they were ordered by distance to the city center so we could consistently generate smaller instances from the whole city. Perimeters were calculated simply by summing each vertex distance from a polygon. Areas were calculated using the Monte Carlo method with 50000 samples. Finally, two BUs are considered neighbors if they share at least two vertices with less than 50 meters of distance.

The output file is in [geojson format](https://geojson.org/) and it is expected to have the following properties:

```json
{
  "features": [
    {
      "geometry": { "polygon-definition-in-longlat-used-to-pretty-print-in-geojson" },
      
      "properties": {
          "ID": "int"
          "CENSUS_ID": "int",
          "AREA": "double",
	  "PERIMETER": "double",
          "DENSITY": "double",
          "POPULATION": "int",
          "NAME": "string",          
          "LIST_ADJACENT": ["int"],
	  "POINTS": [ "polygon-points-projected-considering-reference-point" ]
      },
    }
  ],
  

  "metadata": {
      	"CITY": "string",
	"REFERENCE_LONGLAT": "(double, double)",
	"REFERENCE_XY": "(double, double)",
	"SEED": "double",
	"NB_BLOCKS": "int",
	"NB_POINTS": "int",
	"NB_INHABITANTS": "int",
	"AVG_INHABITANTS": "double",
	"MIN_INHABITANTS": "int",
	"MAX_INHABITANTS": "int",
	"TOT_AREA": "double",
	"AVG_AREA": "double",
	"MIN_AREA": "double",
	"MAX_AREA": "double",
	"MIN_X": "(double, double)",
	"MAX_X": "(double, double)",
	"MIN_Y": "(double, double)",
	"MAX_Y": "(double, double)",
	"GENERATION_TIME_SECONDS": "double"
  },
}
```
## Customer Scenarios

The customers scenarios are generated based on each instance characteristics. We use the following notation to define an instance: ‘city_D_n_t’. Variable city holds for the city name, D is the depot position, n the number of considered BUs and t the target district size. For example 'Machester_C_120_30' represents the instance for Machester with a central depot, number of BUs 120 and target district size 30.

There are two sets of scenarios for each instance, they contain samples of random points for each BU and some information about instance definition. The possible values for variables that define instances are n ∈ {60, 90, 120}, t ∈ {3, 6, 12, 20, 30} and D ∈ {C, NE, NW, SE, SW}. Each value of thas a probability of customer demand, in our experiments it is defined in a way that districts are expected to have 96 demands considering an average population of 8000. Then for each BU, we have generated 50 realizations of the demand (scenarios) for training, and 50 for testing.

The output file is in json format and it is expected to have the following properties:

```json
{
    "blocks": [
        {
	    "ID": "int",
	    "SCENARIOS": [
	    	["(double, double)"]
	    ],
	    "DEPOT_DIST": "double"
	}
    ], 
    
    
    "metadata": {         
	 "PROBA_CUST_DEMAND": "float",         
         "TARGET_SIZE_DISTRICTS": "int",
	 "MAX_SIZE_DISTRICTS": "int",
         "MIN_SIZE_DISTRICTS": "int",
	 "DEPOT_XY": "(double, double)",
	 "DEPOT_LONGLAT": "(double, double)"
    }
}
```
