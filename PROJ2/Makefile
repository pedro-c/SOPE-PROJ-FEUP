CC = gcc
CFLAGS = -D_REENTRANT -lpthread -Wall -pthread

all: parque gerador

Gerador: gerador.c
	$(CC) gerador.c -o gerador $(CFLAGS)

Parque: parque.c
	$(CC) parque.c -o parque $(CFLAGS)

clean:
	rm parque
	rm gerador
