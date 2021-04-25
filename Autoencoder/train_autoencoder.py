# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program used to train the autoencoder network. 
            #  Autoencoder is used to encode game state representations.
import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np
# Disable eager execution
tf.compat.v1.disable_eager_execution()
# Load training data
train_data = pd.read_csv(
    "../data/new_auto_training_data.csv",
    header=1,
    dtype='float32')
# Define the model
autoencoder = tf.keras.Sequential()
autoencoder.add(tf.keras.Input(shape=(33,)))
autoencoder.add(layers.Dense(25))
autoencoder.add(layers.Dense(20))
autoencoder.add(layers.Dense(15))
autoencoder.add(layers.Dense(20))
autoencoder.add(layers.Dense(25))
autoencoder.add(layers.Dense(33))
# Compile the model
autoencoder.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam(), metrics=["accuracy"])
# Fit the model with the training data as both input and output
autoencoder.fit(train_data, train_data, epochs=2)
# Testing code to check accuracy of decodings
    # input_array = train_data[100000:100010]
    # print(np.shape(input_array))
    # prediction = autoencoder.predict(input_array)
    # input_list = np.array(input_array).tolist()
    # for i in range(0, 10):
    #     max_diff = 0.0
    #     for j in range(33):
    #         diff = abs(input_list[i][j] - prediction[i][j])
    #         max_diff = max(max_diff, diff)
    #     print(max_diff)
# Remove decode layers
autoencoder.pop()
autoencoder.pop()
autoencoder.pop()
# The model is now left with just the encoding step
# Save the model
autoencoder.save('../model_data/encoder')
