TARGET=menu
 
# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
 
# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic
 
OBJS=    menu.o
 
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
    
menu.o: src/menu.c
	$(CC) -c $(CCFLAGS) src/menu.c $(GTKLIB) -o menu.o

pending.o: src/pending.c
	$(CC) -c $(CCFLAGS) src/pending.c $(GTKLIB) -o pending.o
    
clean:
	rm -f *.o $(TARGET)
