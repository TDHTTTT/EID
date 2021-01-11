import os
import glob
import csv

input_sizes = {'et_and_ht_and_efpe_and_efph':1, 
               'et_and_ht':1, 
               'et':1, 
               'ht':1, 
               'efpe':1,
               'efph':1,
               'efpe_and_efph':119, 
               'efpe_and_efph_and_hl':1, 
               'hl':1, 
               'et_and_ht_and_hl':119,
               }

features = []

for feature_name in input_sizes.keys():
    features.append(feature_name)

features = glob.glob("*/")

print(features)


for name in features:
    file_paths = glob.glob(name+'/auc/*.csv')
    best_auc_score = 0
    best_model_name = ''
    counter = 0
    for file_path in file_paths:
        with open(file_path) as f:
            reader = csv.reader(f)
            row1 = next(reader)
            auc_score = float(row1[0])
            counter += 1
            if auc_score > best_auc_score:
                best_auc_score = auc_score
                best_model_name = file_path
    print('Num samples', counter)
    print(best_auc_score, name) 
    #print(best_model_name)
    #print('\n')


