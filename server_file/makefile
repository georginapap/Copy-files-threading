SOURCE = data_try_last.c queue.c list.c
OBJS = data_try_last.o queue.o list.o
HEADER = dataServer.h queue.h list.h
OUT = server
CC = gcc
FLAGS = -g -c
LDFLAGS = -pthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LDFLAGS)

data_try_last.o : data_try_last.c
	$(CC) $(FLAGS) data_try_last.c

list.o : list.c
	$(CC) $(FLAGS) list.c

queue.o : queue.c
	$(CC) $(FLAGS) queue.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean : 
	rm -f $(OBJS) $(OUT)

