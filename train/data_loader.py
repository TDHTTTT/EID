import h5py
import numpy as np

def load_data(feature, dset, only_x=False, unscaled=False):
    if unscaled:
        h5_name = 'unscaled_data.h5'
    else:
        h5_name = 'data.h5'
    
    with h5py.File('public_data/'+h5_name, 'r') as hf:
        x = hf['%s/%s'%(feature, dset)]
        x = x[:]
        y = hf['y/%s'%dset]
        y = y[:]

    if only_x:
        return x
    else:
        return [x, y]

def load_combined_data(feature, dset, unscaled=False):
    dataset = []
    if 'et' in feature:
        dataset.append(load_data('et', dset, unscaled=unscaled)[0])
    if 'ht' in feature:
        dataset.append(load_data('ht', dset, unscaled=unscaled)[0])
    if 'hl' in feature:
        dataset.append(load_data('hl', dset, unscaled=unscaled)[0])
    if 'mass' in feature:
        dataset.append(load_data('mass', dset, unscaled=unscaled)[0])
    _, y = load_data('y', dset, unscaled=unscaled)
    return [dataset, y]

if __name__ == "__main__":
    x, y = load_data('mass', 'test')
    print(x.shape, y.shape)
    dataset, y = load_combined_data('et and hl', 'test')
    print(len(dataset))
    for d in dataset:
        print(d.shape)
    dataset, y = load_combined_data('hl_and_mass', 'test')
    print(len(dataset))
    for d in dataset:
        print(d.shape)  
    features = []
    #features.append('hl')
    features.append('et_and_ht')
    features.append('et_and_ht_and_hl')
    features.append('hl_and_mass')
    for feature in features:
        x, y = load_combined_data(feature, 'test')
        print(feature, len(x), type(x))
        for d in x:
            print(d.shape)
        
    
