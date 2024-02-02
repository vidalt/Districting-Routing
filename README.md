# Deep Learning for Data-Driven Districting and Routing

The districting-and-routing problem is a strategic problem in which basic geographical units (e.g., zip codes)
should be aggregated into delivery regions, and each delivery region is characterized by a routing cost
estimated over an extended planning horizon. The objective is to minimize the expected routing costs while
ensuring regional separability through the definition of the districts. We have proposed
to rely on a graph neural network (GNN) trained on a set of demand scenarios, which is then used within
an optimization approach to infer routing costs while solving the districting problem.


<p align="center">
  <img src="https://user-images.githubusercontent.com/24626438/123517730-0a78ae00-d679-11eb-9289-25da0a846fd5.png" width="400">  <img src="https://user-images.githubusercontent.com/24626438/123517713-f5038400-d678-11eb-936e-8358405a8e07.png" width="417">
</p>

Full Paper : **[SOON]**

## Organization of the Repository

* `/Data` contains the data necessary to solve the Districting-and-Routing Problem and the scripts to generate them:
   - [/Data/Dataset](https://github.com/vidalt/Districting/tree/master/Data/Dataset) contains the data related to each city with some test and training customers scenarios.
   - [/Data/TSP-Scenarios](https://github.com/vidalt/Districting/tree/master/Data/TSP-Scenarios) contains the data related randomly generated districts and their expected cost calculated using the Sample Approximation Approach (SAA).

* `/Evaluator` contains scripts used to evaluate solutions
  - [/Evaluator/Compatcness-Evaluation](https://github.com/vidalt/Districting/tree/master/Evaluator/Compatcness-Evaluation) has the code to run a compactness evaluation of a given solution.
  - [/Evaluator/Solution-Cost-Evaluation](https://github.com/vidalt/Districting/tree/master/Evaluator/Solution-Cost-Evaluation) has the code to get the expected costs for a given solution using SAA in the test customers scenarios.

* `/Solver` contains scripts used to evaluate solutions
  - [/Solver/GNN](https://github.com/vidalt/Districting/tree/master/Solver/GNN) has the code related to the Graph Neural Network (GNN) and the trained models.
  - [/Solver/CostEstimators](https://github.com/vidalt/Districting/tree/master/Solver/CostEstimators) has the code for the other cost estimators, namely BD, SNN, and FIG.
  - [/Solver/ILS](https://github.com/vidalt/Districting/tree/master/Solver/ILS) has the code for the Iterated Local Search (ILS) metaheuristic.
  - [/Solver/ExactSolver](https://github.com/vidalt/Districting/tree/master/Solver/ExactSolver) has the code for a formulation to solve the set partitioning problem - able to get optimal in small instances.


## Data Format

All datasets and solutions are written in GeoJSON format.
The format specification is available at this URL: 
http://www.rfc-editor.org/info/rfc7946
