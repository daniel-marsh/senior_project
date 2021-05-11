# Transfer Learning - Daniel Marsh

Project Description:
This project attempts to apply transfer learning to a variety of common machine learning techniques. Each technique is trained on a simplified version of the game "Can't Stop". The agents are then tested on the full version to see how well each approach is able to apply transfer learning.

There are three main approaches. First, a Q-Learning approach that initializes a bucketing scheme using the simplified game and estimates the values of the full game with expected value functions. Second, a LSTM network approach that takes inputs of different sizes. And third, an autoencoder that is trained to translate any version of the game to a common state representation which is then used in training a second network to make game decisions. 

How to run each agent:

The general call to the core program is
$ ./cant_stop [dice_size] [agent 1] [agent 2] [num_games]

Agent Names:

-r : Random

-s : Hard Coded Logic

-q0 : Q-Learning agent with no initailization values

-q1 : Q-Learning agent with initialization values

-l : LSTM agent

-a : Autoencoder agent

For more information about each agent, see the source code in the corresponding directories or the final report write-up in Web_Pages/.

