SOURCES=*.c

EXECUTABLE=pong
CFLAGS=-std=c99 -Wall -L/usr/local/lib -I/usr/local/include
CC=clang
LIBS=-lncurses -lpthread -lfann

all:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
