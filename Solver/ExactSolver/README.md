# Exact Solver for the Districting Problem

This folder contains the code to solve a set partition formulation for the Districting Problem with any cost estimator using [Gurobi Solver](https://www.gurobi.com/). 


Let $\Omega = 2V$ be the set of all possible feasible districts respecting size and
connectivity constraints. Then, the problem can be formulated as the following integer program:

<p align="center">
  <img src="https://user-images.githubusercontent.com/24626438/174694183-c0c29d98-c4e7-4772-9af5-024d8fbdfcdd.png" >
</p>

For each  $ d $, the binary variable $\lambda_d$ takes value 1 if district d is selected in the solution. Parameter $e_{id} = 1$ if BU i appears in district d, and 0 otherwise. Objective (1) corresponds to the total cost of the selected districts. Constraints (2) ensure that each BU is present in one district, and Constraint (3) fixes the number of districts.

## Requirements

Before building the solver it is necessary to have Gurobi 9.1.1 installed (newer versions may work but we do not guarantee) and have the variable ``GUROBI_HOME`` set to your installed Gurobi folder. 

The Makefile is using g++ compiler, if you have another preference it should not be to hard to adapt it.

## Executing

The following command is used to run the program:

```
./exact-solver -f <instance-name> --prediction-method <prediction_method> -port <port-to-http-interface>
```

## Using HTTP interface 

The code expects to have some interface on ``localhost:<port>`` with the following methods allowed:

- POST: accepts string as ``x-www-form-urlencoded`` in request, containing the name of the instance to be loaded, should respond 200 if model was found and loaded in memory.

```
  REQUEST: "Bristol_C_120_3"
  RESPONSE: 200 "Model Loaded"
``` 

- PUT: accepts string as ``x-www-form-urlencoded`` in request, expected to be a string with BU separated by commas and districts by ;, response contains a string with the costs separated by ;. 

```
  REQUEST: "1,2,3,7;10,21,56"
  RESPONSE: 200 "675;120"
``` 

## Expected Output

The program has two files as output, the first one contains concise information about the solution. The name this output file is <instance_name>.<prediction_method>

```
[CITY-NAME]
[FEASIBLE/INFEASIBLE]
COST [ESTIMATED-COST]
EXECUTION TIME [EXECUTION-TIME-IN-SECONDS]
BLOCKS-PER-DISTRICT_SEPARATED-BY-TAB
```

The second produced file is named <instance_name>.<prediction_method>.time.eval and contains details about the execution:

```
ENUM TIME [TIME_TAKEN_TO_ENUMERATE_SOLUTIONS]
COST EVAL TIME [TIME_TAKEN_EVALUATING_COSTS]
MIP TIME [TIME_TAKEN_SOLVING_GUROBI_FORMULA]
TOTAL TIME [TOTAL_EXECUTION_TIME]
```
