import os
os.environ["CUDA_DEVICE_ORDER"]="PCI_BUS_ID";
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
os.environ["CUDA_VISIBLE_DEVICES"]="-1"; 
#%matplotlib inline
import sklearn
from sklearn import metrics
import numpy as np
import keras
from keras.models import Model
from keras.layers import Input, Dense, Activation, Conv2D, MaxPooling2D, Flatten, Dropout
import matplotlib
matplotlib.use('Agg') # prevents from using display
import matplotlib.pyplot as plt
import data_loader
#%matplotlib inline
import tensorflow as tf
tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)
import utils

def roc(true, pred, params):
    """
    Calculates roc curve, roc auc, and saves results.
    true: Numpy array. True labels
    pred: Numpy array. Predicted score
    params: Dictionary. Hyperparameters of the model for saving purposes.  
    returns: Float. ROC AUC score.
    """
    fpr, tpr, _ = metrics.roc_curve(true, pred)
    auc_score = metrics.roc_auc_score(true, pred)
    plt.plot(fpr, tpr, label='%s AUC %f'%(params['feature'], auc_score))
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.legend()
    plt.savefig('%s/plots/'%params['feature']+params['name'] + '_roc_%.4f.png'%auc_score)
    np.savetxt('%s/auc/'%params['feature']+params['name'] + '_auc.csv', np.asarray([auc_score]), delimiter=',')
    np.save('%s/auc/'%params['feature']+params['name'] + '_fpr.npy', np.asarray(fpr))
    np.save('%s/auc/'%params['feature']+params['name'] + '_tpr.npy', np.asarray(tpr)) 
    return auc_score

def conv_block(layer, filters, activation='relu'):
    """
    Convolutional block:
    layer: Keras layer object
    filters: Integer. Number of filters on each convolutional layer
    activation: String. Activation type for the convolutional layer
    returns: Final keras layer of the block.
    """
    layer=Conv2D(filters, kernel_size=(3, 3),activation=activation,padding='same')(layer)
    layer=Conv2D(filters, kernel_size=(3, 3), activation=activation,padding='same')(layer)
    layer=MaxPooling2D(pool_size=(2, 2),padding='same')(layer)
    return layer 

def create_model(params):
    """
    Creates keras model according to specified hyperparameters
    params: Dictionary. Hyperparameters of the model
    returns: Initialized keras model.
    """
    feature = params['feature']
    input_img = []
    flat_layers = []
    towers = []
    
    for pos, input_i in enumerate(params['input_shapes']):
        if len(input_i) == 3:
            input_img.append(Input(shape=input_i, name='image_%i'%pos))
        else:
            flat_layers.append(Input(shape=input_i, name='flat_%i'%pos))
            
    all_inputs = input_img+flat_layers
    
    # Create convolutional towers
    for input_i in input_img: 
        layer = input_i
        for k in range(int(params['numConvBlocks'])):
            layer=conv_block(layer, filters=int(params['filters']))
        towers.append(layer)
    
    # Flatten each convolutional tower
    for tower in towers:
        flat_layers.append(Flatten()(tower))
    
    # Concatenate with EFP or high level features if any
    if len(flat_layers) > 1:
        layer = keras.layers.concatenate(flat_layers, axis=-1)
    else:
        layer = flat_layers[0]
        
    # Fully connected layers
    for k in range(int(params['numLayers'])):
        layer=Dense(int(params['units']), activation='relu')(layer)
        if params['dp'] > 0 and k+1<int(params['numLayers']):
            layer=Dropout(params['dp'])(layer)
    layer=Dense(1, activation='sigmoid')(layer)

    model=Model(all_inputs, layer)

    if params['optimizer'] == 'adam':
        optimizer = keras.optimizers.Adam(lr=params['lr'])   
    model.compile(loss=keras.losses.binary_crossentropy, optimizer=optimizer)
    #model.summary()
    my_model = model
    return my_model

def get_callbacks(params):
    """
    Creates callbacks for keras models
    params: Dictionary. Hyperparameters of the model
    returns: list of keras callbacks
    """
    callbacks = [keras.callbacks.EarlyStopping(monitor='val_loss', min_delta=0, 
                                               patience=10, verbose=0,
                                               mode='auto', baseline=None,
                                               restore_best_weights=True),
                 keras.callbacks.ModelCheckpoint('%s/models/'%feature+params['name'] + '_model.h5', 
                                                monitor='val_loss', verbose=0, save_best_only=True, 
                                                save_weights_only=False, mode='auto', period=1),
                 keras.callbacks.CSVLogger('%s/logs/'%feature+params['name'] + '_log.csv',
                                                     separator=',', append=False),
                 keras.callbacks.ReduceLROnPlateau(monitor='val_loss', factor=0.3, patience=5),
                ]
    return callbacks

def train_model(feature, params, 
                train_x, train_y,
                valid_x, valid_y):
    """
    Train a keras model
    feature: String. Inputs to model, for example high level features, et image, etc.
    params: Dictionary. Hyperparameters of the model.
    train_x: List of numpy arrays or numpy array. Training data input. 
    train_y: Numpy array. Training data label
    valid_x: List of numpy arrays or numpy array. Validation data input. 
    valid_y: Numpy array. Validation data label
    returns: Trained keras model
    """
    
    params['input_shapes'] = utils.get_input_shapes(train_x)          
    print('\n \n Training...')
    print(params['name'], '\n \n')
    model = create_model(params)
    callbacks = get_callbacks(params)
    history = model.fit(train_x, train_y,
                        epochs=params['epochs'], batch_size=params['batchSize'], 
                        validation_data=(valid_x, valid_y),
                        verbose=1,
                        callbacks=callbacks)   
    return model

def test_model(model, test_x, test_y, params):
    """
    Calculates auc on test set.
    model: Keras model
    test_x: List of numpy arrays or numpy array. Test data input. 
    test_y: Numpy array. Test data label
    params: Dictionary. Hyperparameters of the model.
    returns: Float. ROC AUC score
    """
    y_hat = model.predict(test_x)
    y_hat = np.reshape(y_hat,(y_hat.shape[0]))
    auc = roc(test_y, y_hat, params)
    return auc
    
def main(feature, params):
    """
    Trains and tests a keras model for given feature
    feature: String. Specifies which inputs to use.
    params: Dictionary. Hyperparameters for the model.
    returns: None
    """
    
    print(params['feature'])
        
    train_x, train_y = data_loader.load_combined_data(params['feature'], 'train')   
    valid_x, valid_y = data_loader.load_combined_data(params['feature'], 'valid')
        
    params['name'] = utils.create_name(params)
    model = train_model(feature, params, train_x, train_y, valid_x, valid_y) 
    
    test_x, test_y = data_loader.load_combined_data(params['feature'], 'test')
    print('Calculating performance on test set')
    print('AUC', test_model(model, test_x, test_y, params))
    
if __name__ == "__main__":   
    for feature in ['hl', 
                   'et_and_ht', 
                   'et', 
                   'ht',
                   'et_and_ht_and_hl',
                   'mass', 'hl_and_mass',
                   ]:
        utils.create_directories(feature)
        params = utils.get_optimal_params(feature)
        main(feature, params)
