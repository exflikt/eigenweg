import numpy as np
import pandas as pd
import json

JSON = "settings.json"

json_file = open(JSON, 'r')
json_dict = json.load(json_file)
data_file = json_dict['data_file']

# Cramer_Leibniz
# data = pd.read_csv(data_file, delimiter=',', skiprows=[0], header=None).to_numpy()
# Gaussian
data = pd.read_csv(data_file, delimiter=' ', skiprows=[0], header=None).to_numpy()
equation = np.linalg.solve(data[:,:-1], data[:,-1])
for i in equation:
    print('{:.15g}'.format(i), end=" ")