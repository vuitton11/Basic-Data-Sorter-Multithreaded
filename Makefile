CCFLAGS = -ggdb -Wall

all: sorter 

sorter: sorter.c sorter.h directory.h input.h counter.h
	gcc $(CCFLAGS) -pthread -o sorter -g sorter.c sorter.h directory.h input.h counter.h

clean:
	rm -rf sorter

