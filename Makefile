CC = gcc
CFLAGS = -std=c++17 -lstdc++
AGENTS = random.cpp simple.cpp flat_mcts.cpp

all: 

cant_stop: cant_stop.cpp
	$(CC) $(CFLAGS) -o cant_stop cant_stop.cpp $(AGENTS)

clean: 
	$(RM) cant_stop
