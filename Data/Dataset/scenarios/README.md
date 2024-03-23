# DataSet-Scenarios

The files at this folder should have the following format:

## Customer Scenarios

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

## Our Generated Data

You can find our generated TSP data [here](https://www.dropbox.com/scl/fo/cklionmqz1wd6z1c9jw64/h/scenarios-data?dl=0&subfolder_nav_tracking=1).
