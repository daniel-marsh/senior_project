import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np

# Disable eager execution
tf.compat.v1.disable_eager_execution()

raw_data = pd.read_csv(
    "../data/formatted_stop_training_data.csv",
    header=1,
    dtype='float32')

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

encoder = tf.keras.models.load_model('../model_data/encoder')

compressed_input = encoder.predict(padded_input)

# print(compressed_input[:5])

model = tf.keras.Sequential()
# model.add(tf.keras.Input(shape=(15,None)))
model.add(layers.Dense(8))
model.add(layers.Dense(1))

model.compile(loss = tf.losses.BinaryCrossentropy(), optimizer = tf.optimizers.Adam(), metrics=["accuracy"])

model.fit(compressed_input, output, epochs=5)

predictions = model.predict(compressed_input[:5])

print(output[:5])
print(predictions[:5])

model.save('../model_data/stop_compressed')