# Cost Evaluation

This folder contains the code to get the expected cost of a given solution using the Sampling Approximation Approach (SAA).
It is done by solving TSP with Lin–Kernighan algorithm considering training scenarios.

To compile the C++ code just run ``make`` command, it is necessary to have compiler g++ installed.
To execute it:

```
./cost-estimation <city_name> <target_district_size> <depot_position> <number_of_bus> <random_seed> <path_to_solution_file>
```


The input solutions format is described [here](https://github.com/vidalt/Districting/tree/master/Solver/ILS).
The output format is:

```
ESTIMATED COST FOR SOLUTION <float-value>
DEPOT TRIP COST <float-value>
INTRA DISTRICT COST <float-value>
```

It creates a file in ``<path_to_solution_file>.eval``
