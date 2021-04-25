# Author: Daniel Marsh
# Project: Yale CPSC 490 Senior Project
# Description: Program to make a prediction using a saved neural network
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
import numpy as np
import tensorflow as tf
from tensorflow import keras
# Read in the input data
input_data = []
with open("Roll_Network/roll_input.txt", "r") as my_file:
    for line in my_file.readlines():
        num_list = [float(i) for i in line.split(",")]
        input_data.append(num_list)
    my_file.close()
# Format the data for the neural network
input_data = np.array(input_data).astype('float32')
# Load the model
roll_model = keras.models.load_model('./model_data/roll_model')
# Make the predictions
predictions = roll_model.predict(input_data)
# Seperate out each prediction
roll_1_score = predictions[0] + predictions[1]
roll_2_score = predictions[2] + predictions[3]
roll_3_score = predictions[4] + predictions[5]
# Figure out which preditcion is highest
max_roll = max(roll_1_score, roll_2_score, roll_3_score)
# Find the corresponding index
max_roll_ind = 0
if max_roll == roll_2_score:
    max_roll_ind = 1
elif max_roll == roll_3_score:
    max_roll_ind = 2
# Return the index of the pair with the best outcome
with open("Roll_Network/roll_output.txt", "w") as out_file:
    max_roll_out = str(max_roll_ind) + "\n"
    out_file.write(max_roll_out)
    out_file.close()
