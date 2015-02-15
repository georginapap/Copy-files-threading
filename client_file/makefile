SOURCE = remoteClient.c 
OBJS = remoteClient.o 
HEADER = remoteClient.h 
OUT = client
CC = gcc
FLAGS = -g -c

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

remoteClient.o : remoteClient.c
	$(CC) $(FLAGS) remoteClient.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean : 
	rm -f $(OBJS) $(OUT)

