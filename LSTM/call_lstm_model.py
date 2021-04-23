# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program to make predictions using a saved LSTM network

import os
# Silence output
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1' 
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.preprocessing.sequence import pad_sequences

# Read in data used to make prediction
input_data = []
with open("LSTM/lstm_input.txt", "r") as my_file:
    for line in my_file.readlines():
        num_list = [float(i) for i in line.split(",")]
        input_data.append(num_list)
    my_file.close()

# Get the size of the board
num_columns = int(len(input_data[0])/3)
input_data = np.array(input_data)

# Reshape the input
input_features = np.reshape(input_data, (1, num_columns, 3))

# Load the LSTM model
lstm_model = keras.models.load_model('./model_data/lstm_model')

# Make prediction
prediction = lstm_model.predict(input_features)

# Extract expected value
score = prediction[0][0]

# Write out results
with open("LSTM/lstm_output.txt", "w") as out_file:
    score_out = str(score) + "\n"
    out_file.write(score_out)
    out_file.close()
