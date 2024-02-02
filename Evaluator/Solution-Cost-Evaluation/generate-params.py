import os
import pandas as pd
from os import listdir
from os.path import isfile, join


script_dir = os.path.dirname(__file__)
rel_path = "./Solutions"
abs_file_path = os.path.join(script_dir, rel_path)

files = [f for f in listdir(abs_file_path) if isfile(join(abs_file_path, f)) and '120' in f
                                         and (f.endswith('.bd') or f.endswith('.nn'))]

files.sort()

probaCostPerSize = {3: 0.004, 6: 0.002, 12: 0.001, 20: 0.0006, 30: 0.0004}

long_lat_data = {
    'Bristol' : '-2.58791 51.454514',
    'Leeds'   : '-1.54785 53.79648',
    'London'  : '-0.12574 51.50853',
    'Manchester' : '-2.244644 53.483959',
    'West-Midlands' : '-1.89983 52.48142',
}


for fileName in files:
    splited = fileName.split('_')
    cityName = splited[0]
    depotLocation = splited[1]
    sizeSolution = int(splited[2])
    sizeDistrict = int(splited[3].split('.')[0])
    numDistricts = sizeSolution//sizeDistrict

    print("sbatch --export=parameters='" + cityName + " " + long_lat_data[cityName] + " " 
    + str(sizeDistrict) + " 42 " + str(probaCostPerSize[sizeDistrict]) + "  500 1 " 
    + depotLocation + " " + str(sizeSolution) + " 0 " + fileName + "' exec.sh")
