# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program to train the LSTM network. 
# Attribution: This program was adapted from a tutorial project by Christian Versloot at https://www.machinecurve.com/index.php/2021/01/07/build-an-lstm-model-with-tensorflow-and-keras/.
import tensorflow as tf
from tensorflow.keras.datasets import imdb
from tensorflow.keras.layers import Embedding, Dense, LSTM
from tensorflow.keras.losses import BinaryCrossentropy
from tensorflow.keras.models import Sequential
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.preprocessing.sequence import pad_sequences
import pandas as pd
import numpy as np

# Set up model configuration values
additional_metrics = ['accuracy']
batch_size = 64
loss_function = BinaryCrossentropy()
number_of_epochs = 20
optimizer = Adam()
validation_split = 0
verbosity_mode = 1
# Disable eager execution
tf.compat.v1.disable_eager_execution()
# Load in the raw data
data_train = pd.read_csv(
    "../data/formatted_stop_training_data.csv",
    header=0,
    dtype='float32')
# Seperate the input and output data
training_input = data_train.copy()
training_output = training_input.pop('OUTPUT')
training_input = np.array(training_input)
# See how many training examples we have
num_training_cases = training_input.shape[0]
# Check how many columns the board has
num_columns = int(training_input.shape[1]/3)
# Reshape the input data to feed into the LSTM network
training_input = np.reshape(training_input, (num_training_cases, num_columns, 3))
# Define the model
model = Sequential()
model.add(LSTM(8, input_shape=(None,3)))
model.add(Dense(1, activation='sigmoid'))
# Compile the model
model.compile(optimizer=optimizer, loss=loss_function, metrics=additional_metrics)
# Train the model
model.fit(training_input, training_output, batch_size=batch_size, epochs=number_of_epochs, verbose=verbosity_mode, validation_split=validation_split)
# Save the model
model.save('../model_data/test_lstm_model')
