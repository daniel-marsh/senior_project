# Transfer Learning - Daniel Marsh

Project Description:
This project will attempt to apply transfer learning to a variety of common machine learning techniques. Each technique will be trained on a simplified version of the game "Can't Stop". The agents will then be tested on the full version to see how well each approach is able to apply transfer learning.

There will be three main approaches. First, a Q-Learning approach that initializes a bucketing scheme using the simplified game and estimates the values of the full game with expected value functions. Second, a LSTM network approach that takes inputs of different size. And third, an autoencoder that is trained to translate any version of the game to a common state representation. 

How to run each agent:

The general call is $ ./cant_stop [dice_size] [agent 1] [agent 2] [num_games]

Agent Names:
-r : Random
-s : Hard Coded Logic
-m : Simple MCTS agent
-q0 : Q-Learning agent with no initailization values
-q1 : Q-Learning agent with initialization values
-l : LSTM agent

LSTM Agent info:

All training data is stored in .csv files. 
Data for the roll network is generated by load_data.cpp and stored in roll_training_data.csv. The roll network is then trained with roll_net.py and called for predictions with call_roll_network.py.
Data for the LSTM network is generated by load_lstm_data.cpp, formatted by format_lstm_data.cpp, and stored in formatted_stop_training_data.csv. The LSTM network is then trained with train_lstm.py and called for predictions with call_lstm_model.py.


