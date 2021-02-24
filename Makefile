CC = gcc
CFLAGS = -std=c++17 -lstdc++

all: 

cant_stop: cant_stop.cpp
	$(CC) $(CFLAGS) -o cant_stop cant_stop.cpp

clean: 
	$(RM) cant_stop
