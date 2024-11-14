CC = gcc
CFLAGS = -Wall -std=c99 -g -pedantic -Werror -Wextra -Wvla -D_DEFAULT_SOURCE
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: myfind

myfind: $(OBJ)
	$(CC) $(CFLAGS) -o myfind $^

check: all
	./tests/testsuite.sh

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	$(RM) src/*.o myfind check

.PHONY: all clean check
