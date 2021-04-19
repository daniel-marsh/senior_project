import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
import numpy as np
import tensorflow as tf
from tensorflow import keras

# Load in input data
input_data = []
with open("autoencoder_input.txt", "r") as my_file:
    for line in my_file.readlines():
        num_list = [float(i) for i in line.split(",")]
        while len(num_list) < 33:
            num_list.append(-1.0)
        input_data.append(num_list)
    my_file.close()
input_data = np.array(input_data).astype('float32')

# Load models
encoder = keras.models.load_model('./model_data/encoder')
model = keras.models.load_model('./model_data/stop_compressed')
# Get state encoding
compressed_data = encoder.predict(input_data)
# Get move suggestion
prediction = model.predict(compressed_data)

with open("autoencoder_output.txt", "w") as out_file:
    str_out = str(prediction[0][0])
    out_file.write(str_out)
    out_file.close()
