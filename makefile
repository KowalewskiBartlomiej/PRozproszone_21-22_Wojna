CC = mpic++
CFLAGS = -Wall

war:
	$(CC) $(CFLAGS) -o war main.cpp message_handler.cpp structs.cpp

clean: 
	rm war 