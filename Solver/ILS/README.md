# Iterative Local Search for the Districting Problem

This repository has an ILS Metaheuristic to solve the Districting problem. It can estimate the solution cost with Beardwood Daganzo Upper Bound or using some generic HTTP interface. Our interest is to evaluate the usage of Neural Nets to evaluate the expected route costs where demand is uncertain, but the interface can be used to evaluate other estimators as long as it follow some directives. More details in: [Using HTTP interface section](#using-http-interface). Also, it is easy to create a new class that implements the CostEvaluator interface and plug it into ILS.


The problem consists of finding the grouping of Basic Units (BU) areas that have minimum expected routing cost. 


Example of an Instance and some feasible solution


  <img src="https://user-images.githubusercontent.com/24626438/123517730-0a78ae00-d679-11eb-9289-25da0a846fd5.png" width="400">  <img src="https://user-images.githubusercontent.com/24626438/123517713-f5038400-d678-11eb-936e-8358405a8e07.png" width="417">


File formatting is detailed in [Expected Input and Output section](#expected-input-and-output).


## Executing

### Requirements

Before building the solver it is necessary to have Gurobi 9.1.1 installed (newer versions may work but we do not guarantee) and have the variable GUROBI_HOME set to your installed Gurobi folder. 

The Makefile is using g++ compiler, if you have another preference it should not be to hard to adapt it.

To run our Neural Net models, checkout [Learning](https://github.com/vidalt/Districting/tree/master/learning).

## Compiling

Go to src folder and run ``make``, it should create .o files into the build folder and the executable ``districting_solver`` at the main folder.

## Running

You can run a example with:

```
./districting_solver -f Bristol_C_120_12 
```

The expected call to solver should have:
```
./districting_solver -f instanceName [-output solPath] [-p probabilitySkipMove] [-t timeLImitInSeconds]
                     [-port portToRequestCostEstimation] [--prediction-method nameOfPredictionMethod]
                     [-seed randomSeed] 
```

 - instanceName: mandatory parameter with the name of the instance. The solver will consider the folder ``../../Data/Dataset`` to look for the instance geojson file. It must have format ``{cityName}_{depotPosition}_{instanceSize}_{targetDistrictSize}``.
 - output      : optional parameter with the path for the output solution file. Default value is ``{instancePath.nameOfPredictionMethod}``.
 - probabilitySkipMove : optional parameter with the hyperparameter for the Perturbation in ILS. Default value is 0.99.
 - timeLImitInSeconds: optional parameter with the time in seconds that ILS will run. Default value is 60s.
 - portToRequestCostEstimation: optional parameter with the port thar method will cost estimation in localhost. Default value is 8000.
 - prediction-method: name of the prediction method. Used only for producing the solution file name. Default value is ``nn``.
 - randomSeed: optional parameter with the value to initialize random number generator

## Expected Input and Output

### Inputs

The are two input files. The first one is expected to be in ``../../Data/Dataset/{cityName}.geojson``. It should be in [geojson format](https://geojson.org/) and it is expected to have the following properties:

```json
{
  "features": [
    {
      "geometry": { "optional-used-for-visualization" },
      
      "properties": {
          "ID": "int",
          "AREA": "float",
          "DENSITY": "float",
          "POPULATION": "int",
          "NAME": "string",
          "LIST_ADJACENT": ["int"],
      },
    }
  ],
 
}
```
 - ID: number that uniquely identifies a Basic Unit (BU)
 - LIST_ADJACENT: list of BU's ids that share a border

The second file is in json format and is expected to be in ``../../Data/Dataset/scenarios/{instanceName}.train.json``. It should have the following properties:
 
 ```json
{
  "blocks": [
    {
       "ID": "int",
       "DEPOT_DIST": "float",
    }
  ],
  

  "metadata": {
    "PROBA_CUST_DEMAND": "float",         
    "TARGET_SIZE_DISTRICTS": "int",
    "MAX_SIZE_DISTRICTS": "int",
    "MIN_SIZE_DISTRICTS": "int",
    "NUMBER_OF_DISTRICTS": "int",
  },
}

```
 - DIST_DEPOT: distance of a given BU to the depot
 - NUMBER_OF_DISTRICTS: desirable size of each district
 - MIN_SIZE_DISTRICTS: minimum size of a district. Maximum will be calculated as:
 - MAX_SIZE_DISTRICTS: minimum size of a district. Maximum will be calculated as:

### Outputs

There are two output files. One is a geojson really similar the input, but it has some aditional properties:

```json
{
  "features": [
    {
      
      "properties": {
         "ALL_INPUT_PROPERTIES_PER_BU" : "input-types",
         "DISTRICT_ID": "district id block is inserted",
         "stroke": "color",
         "stroke-width": 3,
         "stroke-opacity": 1.25,
         "fill": "color",
         "fill-opacity": "0.65",
      },
    }
  ],
}
```

Besided the district id, all other properties are used to have a good visualization of the output solution.

Also, the solver generates a file that contains more concise information about the solution:

```
[CITY-NAME]
[FEASIBLE/INFEASIBLE]
Cost: [ESTIMATED-COST]
BLOCKS-PER-DISTRICT_SEPARATED-BY-TAB
```

For example:

```
Bristol_C_60_3
feasible
Cost: 735.237
22	54	17	31	
46	57	41	33	
45	43	32	34	
53	30	
59	26	
52	29	42	14	
36	48	
58	50	
9	20	
19	6	
56	49	
37	25	40	
24	44	39	51	
13	16	11	7	
28	35	47	18	
4	0	15	1	
23	3	8	55	
38	27	
5	10	
21	12	2	
```

## Using HTTP interface 

The solver expects to have an server responding in ``localhost:{port|8000}`` with the following methods allowed:

- POST: accepts string as ``x-www-form-urlencoded`` in request, containing the name of the instance to be loaded, should respond 200 if model was found and loaded in memory

```
  REQUEST: "Bristol_C_120_3"
  RESPONSE: 200 "Model Loaded"
``` 

- PUT: accepts string as ``x-www-form-urlencoded`` in request, expected to be a string with BU separated by commas and districts by ;, response contains a string with the costs separated by ;. 

```
  REQUEST: "1,2,3,7;10,21,56"
  RESPONSE: 200 "675;120"
``` 


