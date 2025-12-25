CC = gcc
CFLAGS = -Iinclude -I/usr/include/postgresql
LDFLAGS = -lpq -lpthread

SRCS = server.c $(wildcard src/*.c) $(wildcard src/controllers/*.c) $(wildcard src/dao/*.c) $(wildcard src/services/*.c)
OBJS = $(SRCS:.c=.o)

all: server

server: $(OBJS)
	$(CC) -o server $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f server $(OBJS)