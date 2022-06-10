CC = mpic++
CFLAGS = -Wall

war:
	$(CC) $(CFLAGS) -o war main.cpp message_handler.cpp ship.cpp structs.h

clean: 
	rm war 