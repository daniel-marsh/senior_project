import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1' 
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.preprocessing.sequence import pad_sequences

input_data = []
with open("lstm_input.txt", "r") as my_file:
    for line in my_file.readlines():
        num_list = [float(i) for i in line.split(",")]
        input_data.append(num_list)
    my_file.close()

max_sequence_length = 22
padded_inputs = pad_sequences(input_data, maxlen=max_sequence_length, value = 0.0)

lstm_model = keras.models.load_model('./model_data/lstm_model')

# Prediction will be a list of lists with only one total entry
prediction = lstm_model.predict(padded_inputs)

score = prediction[0][0]

with open("lstm_output.txt", "w") as out_file:
    score_out = str(score) + "\n"
    out_file.write(score_out)
    out_file.close()
