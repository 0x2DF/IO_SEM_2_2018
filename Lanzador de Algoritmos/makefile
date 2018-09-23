# target
TARGET=menu
# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

CCFLAGS=$(DEBUG) $(OPT) $(WARN)
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0` -rdynamic
 
# linker
LD=gcc
LDFLAGS=$(GTKLIB) -export-dynamic
 
OBJS=    menu.o
 
all: $(OBJS)
	$(CC) src/menu.c -o $(TARGET) $(CCFLAGS) $(LDFLAGS)
	$(CC) -o pending src/pending.c $(CCFLAGS) $(LDFLAGS)
	$(CC) -o floyd src/floyd.c $(CCFLAGS) $(LDFLAGS)
	$(CC) -o knapsack src/knapsack.c $(CCFLAGS) $(LDFLAGS)
	$(CC) -o series src/series.c $(CCFLAGS) $(LDFLAGS)

menu.o: src/menu.c
	$(CC) -c $(CCFLAGS) src/menu.c $(GTKLIB) -o menu.o

clean:
	rm -f *.o $(TARGET)
