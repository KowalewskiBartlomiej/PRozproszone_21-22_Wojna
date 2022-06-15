CC = mpic++
CFLAGS = -Wall -lpthread -std=c++0x

war:
	$(CC) $(CFLAGS) -o war main.cpp message_handler.cpp ship.cpp structs.h

clean: 
	rm war 