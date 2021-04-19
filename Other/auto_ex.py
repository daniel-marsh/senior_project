import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np

# Disable eager execution
tf.compat.v1.disable_eager_execution()

x_train = pd.read_csv(
    "formatted_stop_training_data.csv",
    header=0,
    dtype='float32')

autoencoder = tf.keras.Sequential()
autoencoder.add(tf.keras.Input(shape=(16,)))
autoencoder.add(layers.Dense(13))
autoencoder.add(layers.Dense(10))
autoencoder.add(layers.Dense(13))
autoencoder.add(layers.Dense(16))

autoencoder.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam(), metrics=["accuracy"])
autoencoder.fit(x_train, x_train, epochs=15)

autoencoder.pop()
autoencoder.pop()

predictions = autoencoder.predict(x_train[:5])
print(predictions)
