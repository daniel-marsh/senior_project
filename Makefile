CC = gcc
CFLAGS = -std=c++17 -lstdc++
AGENTS = random.cpp simple.cpp flat_mcts.cpp lstm.cpp

all: 

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
	
clean: 
	$(RM) cant_stop
