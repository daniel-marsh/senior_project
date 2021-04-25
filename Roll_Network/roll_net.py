# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program to train neural network to choose best pairings of dice for Can't Stop
# Attribution: This program was inspired by several tutorials freely available at tensorflow.org
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.layers.experimental import preprocessing
# Make numpy values easier to read.
np.set_printoptions(precision=3, suppress=True)
# Load in the training data
roll_train = pd.read_csv(
    "../data/roll_training_data.csv",
    header=0,
    dtype='float32')
# Seperate the input and output data
roll_input = roll_train.copy()
roll_output = roll_input.pop('Score Diff')
# Format the input for training
roll_input = np.array(roll_input)
# Define the model
roll_model = tf.keras.Sequential([
  layers.Dense(128),
  layers.Dense(1)
])
# Compile the model
roll_model.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam())
# Fit the model
roll_model.fit(roll_input, roll_output, epochs=10)
# Make a few test predictions
prediction = roll_model.predict(roll_input[:5])
print(prediction)
# Save the model
roll_model.save('../model_data/roll_model')
