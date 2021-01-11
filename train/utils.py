import os


def create_directories(directory):
    """
    Creates directories for saving models if they don't already exist
    directory: Directory name, typically it will be the feature set
    returns: None
    """
    if not os.path.exists(directory):
        os.makedirs(directory)
        os.makedirs(directory+'/models')
        os.makedirs(directory+'/auc')
        os.makedirs(directory+'/logs')
        os.makedirs(directory+'/plots')
        
def get_input_shapes(train_x):
    """
    Returns tuple with shapes for keras model in case of multiple inputs
    train_x: List of numpy arrays or single numpy array. Input to Keras model
    """
    shapes = []
    if type(train_x) == list:
        for i in range(len(train_x)):
            shapes.append(tuple(train_x[i].shape[1:]))
    else:
        shapes.append(tuple(train_x.shape[1:]))
    return tuple(shapes)
               
def append_param_to_str(name_list, param, params):
    if len(name_list)>0:
        name_list.append('_')
    name_list.append(param)
    name_list.append('_')
    if param == 'lr' or param=='p' or param=='dp':
        name_list.append('%.4f'%params[param])
    else:
        name_list.append('%s'%str(params[param]))
    return name_list

def create_name(params):
    """
    Creates name for model from hyperparameters for saving purposes
    params: Dictionary of hyperparameters
    returns: String. Name for model.
    """
    name_list = []  
    parameters_in_name = ['feature', 
                  'numLayers', 'units',
                  'lr',
                  'dp',
                  'epochs']
    if 'et' in params['feature'] or 'ht' in params['feature']:
        parameters_in_name.append('numConvBlocks')
        parameters_in_name.append('filters')
    for param in parameters_in_name:
        append_param_to_str(name_list, param, params) 
    name = "".join(name_list)
    name = name.replace(" ", "_")
    return name 
        

def get_optimal_params(feature):
    """
    Returns dictionary with optimal hyperparams
    feature: String with feature name
    returns: Dictionary with optimal hyperparameters
    """
    params = {'feature':feature,
              'filters':16,
              'numConvBlocks':1,
              'p':0,
              'optimizer':'adam',
              'epochs':100,
              'batchSize':128,
              'iso_positions':(),
              'efp_positions':(),
             }
    if feature == 'et':
        params['numLayers']=2
        params['units']=160
        params['lr']=0.0001
        params['dp']=0.0
        params['filters']=117
        params['numConvBlocks']=3
    elif feature == 'ht':
        params['numLayers']=2
        params['units']=84
        params['lr']=0.01
        params['dp']=0.5
        params['filters']=27
        params['numConvBlocks']=2
    elif feature == 'et_and_ht':   
        params['numLayers']=2
        params['units']=146
        params['lr']=0.0001
        params['dp']=0.0
        params['filters']=47
        params['numConvBlocks']=3
    elif feature == 'et_and_ht_and_hl': 
        params['numLayers']=2
        params['units']=154
        params['lr']=0.0001
        params['dp']=0.0
        params['filters']=34
        params['numConvBlocks']=3
    elif feature == 'hl':
        params['numLayers']=5
        params['units']=149
        params['lr']=0.001
        params['dp']=0.0019
    elif feature == 'hl_and_mass':
        params['numLayers']=3
        params['units']=109
        params['lr']=0.0013
        params['dp']=0.0
    elif feature == 'mass':
        params['numLayers']=3
        params['units']=10
        params['lr']=0.01
        params['dp']=0.0
    else:
        assert False, params['feature']
    return params