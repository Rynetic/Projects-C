CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla -fPIC -fvisibility=hidden
LDFLAGS = -shared -nostartfiles -static-libgcc

VPATH = src
TARGET_LIB = libmalloc.so
SRCS = malloc.c my_malloc.c calloc.c free.c
OBJS = $(SRCS:.c=.o)

all: library

library: $(TARGET_LIB)

$(TARGET_LIB): CFLAGS += -pedantic
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -Wl,--version-script=malloc.map
$(TARGET_LIB): malloc.map $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

malloc.map:
	@echo "{ global: malloc; free; calloc; realloc; local: *; };" > $@
check: library
	@cd tests && ./test.sh

clean:
	$(RM) $(TARGET_LIB) $(OBJS) malloc.map

.PHONY: all library clean check
