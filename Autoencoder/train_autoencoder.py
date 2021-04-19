import tensorflow as tf
from tensorflow.keras import layers
import pandas as pd
import numpy as np

# Disable eager execution
tf.compat.v1.disable_eager_execution()

x_train = pd.read_csv(
    "../data/auto_training_data.csv",
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

autoencoder.pop()
autoencoder.pop()
autoencoder.pop()

autoencoder.save('../model_data/encoder')