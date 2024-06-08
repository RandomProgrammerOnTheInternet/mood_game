CC = gcc
CCFLAGS =
LDFLAGS =
INCFLAGS = -iquotesrc -I.
CCFLAGS += -std=gnu2x
CCFLAGS += -g
CCFLAGS += -O2
CCFLAGS += -fsanitize=undefined,address,leak
CCFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)
LDFLAGS += -lGL
LDFLAGS += -Wall
LDFLAGS += -lm
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: build

%.o: %.c
	$(CC) $(CCFLAGS) $(INCFLAGS) -o $@ -c $<

build: $(OBJ)
	$(CC) $(INCFLAGS) $(CCFLAGS) $(LDFLAGS) $(OBJ) -o main
