import pandas as pd
import numpy as np

# Make numpy values easier to read.
np.set_printoptions(precision=3, suppress=True)

import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.layers.experimental import preprocessing

roll_train = pd.read_csv(
    "roll_training_data.csv",
    names=["Stop", "Runner", "Free", "Score"])

roll_features = roll_train.copy()
roll_labels = roll_features.pop('Score')

roll_features = np.array(roll_features)

roll_model = tf.keras.Sequential([
  layers.Dense(2048),
  layers.Dense(1)
])

roll_model.compile(loss = tf.losses.MeanSquaredError(), optimizer = tf.optimizers.Adam())

roll_model.fit(roll_features, roll_labels, epochs=25)

prediction = roll_model.predict(roll_features[:5])

print(prediction)

roll_model.save('./model_data/roll_model')