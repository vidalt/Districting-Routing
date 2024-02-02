# Cost Estimators

This folders contains the code to train and get prediction from the BD, FIG and SNN methods.

## Requirements and Installation
 
In order to executed the code you should have Python 3 and pip installed.
It is also necessary to have python virtualenv installed. To check for installation:

```sh
virtualenv --version
```

If is not installed just run
```sh
pip install virtualenv
```

Then it is necessary to create python env, activate it and install libraries:

```sh
python -m venv ./venv;
source ./venv/bin/activate;
pip install -r requirements;
```

## Training

To train the model for every of the three methods, execute:

```
python train_model.py <instance-name>
```

The scripts expects to have the following files:

- ``../../Data/Dataset/<city-name>.geojson`` with city data.
- ``../../Data/Dataset/scenarios/<instance-name>.train.json`` with data relative to train scenarios.
- ``../../Data/TSP-Scenarios/districts-with-costs/<instance-name>_tsp.train.json`` with samples of districts and its expected costs.

The outputs are two files for each model:

- ``best.model`` containing the pickle for the best model found for a given (method, instance).
- ``training-data.json`` containing the training metrics.

## Serving Prediction

To start the server that runs predictions:

```
python -p <port> -model_type <BD|FIG|SNN>
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

