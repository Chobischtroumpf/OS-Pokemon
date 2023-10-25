CC=gcc
OPT=-Wall -Wextra -O2 -Wpedantic
OBJS=

all: img-search

img-search: main.c $(OBJS)
	$(CC) $(OPT) $(OPT) main.c utils.c -o img-search $(OBJS)

%.o: %.c %.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@

clean:
	rm -f img-search $(OBJS)