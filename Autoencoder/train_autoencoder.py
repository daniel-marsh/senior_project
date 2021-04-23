import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np

np.set_printoptions(threshold=np.inf)
# Disable eager execution
tf.compat.v1.disable_eager_execution()

x_train = pd.read_csv(
    "../data/new_auto_training_data.csv",
    header=1,
    dtype='float32')

autoencoder = tf.keras.Sequential()
autoencoder.add(tf.keras.Input(shape=(33,)))
autoencoder.add(layers.Dense(25))
autoencoder.add(layers.Dense(20))
autoencoder.add(layers.Dense(15))
autoencoder.add(layers.Dense(20))
autoencoder.add(layers.Dense(25))
autoencoder.add(layers.Dense(33))

autoencoder.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam(), metrics=["accuracy"])
autoencoder.fit(x_train, x_train, epochs=2)

input_array = x_train[100000:100010]

print(np.shape(input_array))

prediction = autoencoder.predict(input_array)
input_list = np.array(input_array).tolist()
for i in range(0, 10):
    max_diff = 0.0
    for j in range(33):
        diff = abs(input_list[i][j] - prediction[i][j])
        max_diff = max(max_diff, diff)
    print(max_diff)


autoencoder.pop()
autoencoder.pop()
autoencoder.pop()

autoencoder.save('../model_data/encoder')



