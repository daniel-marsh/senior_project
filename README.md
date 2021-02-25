# Transfer Learning - Daniel Marsh

Project Description:
This project will attempt to apply transfer learning to a variety of common machine learning techniques. Each technique will be trained on a simplified version of the game "Can't Stop". The agents will then be tested on the full version to see how well each approach is able to apply transfer learning.

There will be three main approaches. First, a Q-Learning approach that initializes a bucketing scheme using the simplified game and estimates the values of the full game with expected value functions. Second, a LSTM network approach that takes inputs of different size. And third, an autoencoder that is trained to translate any version of the game to a common state representation. 

How to run each agent:

The general call is $ ./cant_stop [dice_size] [agent 1] [agent 2]


