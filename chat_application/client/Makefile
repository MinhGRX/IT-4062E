CC = gcc
CFLAGS = -Iinclude -Wall
LIBS = -lpthread

SRCS = src/main.c src/network.c
OBJS = $(SRCS:.c=.o)

client: $(OBJS)
	$(CC) $(OBJS) -o client $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f client src/*.o