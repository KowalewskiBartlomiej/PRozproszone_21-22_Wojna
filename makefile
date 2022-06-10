CC = mpic++
CFLAGS = -Wall

war: main.cpp message_handler.cpp structs.cpp
	$(CC) $(CFLAGS) -o main.cpp message_handler.cpp structs.cpp