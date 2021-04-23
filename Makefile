CC = gcc
CFLAGS = -std=c++17 -lstdc++
AGENTS = Simple_Agents/random.cpp Simple_Agents/simple.cpp Simple_Agents/flat_mcts.cpp LSTM/lstm.cpp Autoencoder/autoencoder.cpp

all: cant_stop test_link_c load_data load_lstm_data format_lstm_data get_auto_data

cant_stop: cant_stop.cpp
	$(CC) $(CFLAGS) -o cant_stop cant_stop.cpp $(AGENTS)

test_link_c: test_link_c.cpp
	$(CC) $(CFLAGS) -o test_link_c test_link_c.cpp

load_data: load_data.cpp
	$(CC) $(CFLAGS) -o load_data load_data.cpp

load_lstm_data: load_lstm_data.cpp
	$(CC) $(CFLAGS) -o load_lstm_data load_lstm_data.cpp

format_lstm_data: format_lstm_data.cpp
	$(CC) $(CFLAGS) -o format_lstm_data format_lstm_data.cpp

get_auto_data: Autoencoder/get_auto_data.cpp
	$(CC) $(CFLAGS) -o get_auto_data Autoencoder/get_auto_data.cpp
	
clean: 
	$(RM) all
