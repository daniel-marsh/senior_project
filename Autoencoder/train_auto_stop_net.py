# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program used to train the network that makes move predictions based on encoded game states. 
import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np
# Disable eager execution
tf.compat.v1.disable_eager_execution()
# Load in training data
raw_data = pd.read_csv(
    "../data/formatted_stop_training_data.csv",
    header=1,
    dtype='float32')
# Format data for input to encoder
raw_data = np.array(raw_data)
padded_input = []
output = []
for i in range(0, len(raw_data)):
    line = []
    for elem in raw_data[i]:
        line.append(elem)
    output.append([line[15]])
    line = line[:-1]
    while len(line) < 33:
        line.append(-1.0)
    padded_input.append(line)
output = np.array(output)
padded_input = np.array(padded_input)
# Load the encoder network
encoder = tf.keras.models.load_model('../model_data/encoder')
# Encode the training inputs
compressed_input = encoder.predict(padded_input)
# Define the prediction model
model = tf.keras.Sequential()
model.add(layers.Dense(8))
model.add(layers.Dense(1))
# Compile the prediction model
model.compile(loss = tf.losses.BinaryCrossentropy(), optimizer = tf.optimizers.Adam(), metrics=["accuracy"])
# Fit the prediction model
model.fit(compressed_input, output, epochs=5)
# Make some mock predictions
predictions = model.predict(compressed_input[:5])
print(output[:5])
print(predictions[:5])
# Save the predictor model
model.save('../model_data/stop_compressed')
