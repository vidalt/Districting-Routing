# Convolutional Neural Networks for Districting 

This document is in WIP state, some sections may be missing.


## Requirements

In order to use that code you should have Python 3 and pip installed.
It is also recommender to have python virtualenv installed. To check for installation:

```sh
virtualenv --version
```

If is not installed just run
```sh
pip install virtualenv
```


## Installation

First it is necessary to compile the s2v_lib. To do that, just enter in the s2v_lib folder and execute make command.
Then it is necessary to create python env, activate it and install libraries:

```sh
python -m venv ./venv;
source ./venv/bin/activate;
pip install -r requirements;
```

## Training

To train the model execute:

```
python main.py -city_name <city-name> -depot_position <depot-position> -instance_size <instance_size> \
               -target_size_district <target_district_size> -traning 1
```

If training parameter is not defined the code will evaluate the testing set for the given instance.

The scripts expects to have the following files:

- ``../../Data/Dataset/<city-name>.geojson`` with city data.
- ``../../Data/Dataset/scenarios/<instance-name>.train.json`` with data relative to train scenarios.
- ``../../Data/TSP-Scenarios/districts-with-costs/<instance-name>_tsp.train.json`` with samples of districts and its expected costs.

The outputs are two files for each model:

- ``epoch-best.model`` containing the pickle for the best model found for a given (method, instance).
- ``training-data.json`` containing the training metrics.
- ``log_training.png`` with the graphic of loss functions amongst iterations.

The are several other optional parameters:

- ``-seed`` random seed, default value 1.
- ``-n_epoch`` number of epochs to train model, default value 10.000.
- ``-learning_rate`` learning rate, default value 0.0001.
- ``-mode`` execution mode, cpu or gpu. Default value is cpu (to run GPU is necessary to have CUDA installed).
- ``-gm`` [TODO]
- ``-batch_size`` the minibatch size, default value is 64.
- ``-feat_dim`` dimension of node features, default values is 0.
- ``-num_class`` number of classes (only for classification problems), default value is 0.
- ``-fold`` number of folds in cross validation, default value is 1.
- ``-latent_dim`` dimension of latent layers, default value is 64.
- ``-out_dim`` s2v output size, default value is 1024.
- ``-hidden`` dimension of the hidden layer in regression, default value is 100.
- ``-max-lv`` max rounds of messagens in latent layers, default value is 4.


## Serving Prediction

To start server that runs predictions:

```sh
python start-server.py -p <port>;
```

If none port is given, the server starts in 8000. It accepts the following methods:

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

The example above can be interpreted as: district with BUs {1, 2, 3, 7} has 675 as expected cost, and district {10, 21, 56} as 120.
