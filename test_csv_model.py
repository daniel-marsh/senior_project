import pandas as pd
import numpy as np

# Make numpy values easier to read.
np.set_printoptions(precision=3, suppress=True)

import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.layers.experimental import preprocessing

abalone_train = pd.read_csv(
    "https://storage.googleapis.com/download.tensorflow.org/data/abalone_train.csv",
    names=["Length", "Diameter", "Height", "Whole weight", "Shucked weight",
           "Viscera weight", "Shell weight", "Age"])

abalone_features = abalone_train.copy()
abalone_labels = abalone_features.pop('Age')

abalone_features = np.array(abalone_features)

abalone_model = tf.keras.Sequential([
  layers.Dense(2048),
  layers.Dense(1)
])

abalone_model.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam())

abalone_model.fit(abalone_features, abalone_labels, epochs=25)

prediction = abalone_model.predict(abalone_features[:5])

print(prediction)