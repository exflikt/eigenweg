import numpy as np
import pandas as pd
import json

JSON = "settings.json"

json_file = open(JSON, 'r')
json_dict = json.load(json_file)
data_file = json_dict['data_file']

data = pd.read_csv(data_file, delimiter=' ', skiprows=[0], header=None).to_numpy()
determinant = np.linalg.det(data)
print('{:.15g}'.format(determinant))