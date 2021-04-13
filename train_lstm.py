import tensorflow as tf
from tensorflow.keras.datasets import imdb
from tensorflow.keras.layers import Embedding, Dense, LSTM
from tensorflow.keras.losses import BinaryCrossentropy
from tensorflow.keras.models import Sequential
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.preprocessing.sequence import pad_sequences

import pandas as pd
import numpy as np

# Model configuration
additional_metrics = ['accuracy']
batch_size = 256
embedding_output_dims = 15
loss_function = BinaryCrossentropy()
max_sequence_length = 33
num_distinct_words = 305
number_of_epochs = 5
optimizer = Adam()
validation_split = 0
verbosity_mode = 1

# Disable eager execution
tf.compat.v1.disable_eager_execution()

# Load dataset
data_train = pd.read_csv(
    "formatted_stop_training_data.csv",
    header=0,
    dtype='float32')

training_features = data_train.copy()
training_labels = training_features.pop('OUTPUT')

training_features = np.array(training_features)

# Pad all sequences
padded_inputs = pad_sequences(training_features, maxlen=max_sequence_length, value = 0.0) # 0.0 because it corresponds with <PAD>

# print(padded_inputs[0])

# Define the Keras model
model = Sequential()
model.add(Embedding(num_distinct_words, embedding_output_dims, input_length=max_sequence_length))
model.add(LSTM(25))
model.add(Dense(1, activation='sigmoid'))

# Compile the model
model.compile(optimizer=optimizer, loss=loss_function, metrics=additional_metrics)

# Give a summary
model.summary()

# Train the model
history = model.fit(padded_inputs, training_labels, batch_size=batch_size, epochs=number_of_epochs, verbose=verbosity_mode, validation_split=validation_split)

# See some predictions
predictions = model.predict(padded_inputs[:25])

print("EXPECTED")
print(training_labels[:25])

print("PREDICTIONS")
print(predictions[:25])

model.save('./model_data/lstm_model')