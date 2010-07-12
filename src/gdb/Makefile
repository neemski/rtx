# Makefile
# David Grant, 2004
# Irene Huang, 2010/05/03 
# Mikhail Zakharov, 2010/05/11

#where all the headders are
INC=-I../common/

#uncomment this to build a library for cf-server
#DEFINES+=-D_CFSERVER_
DEFINES+=-D_GDB_

#optimization level
OPT=-O0

OBJS=m68k-stub.o setjmp.o gdb_uart.o queue.o

CFLAGS=-Wall $(OPT) -Werror -m5307 -pipe -nostdlib ${INC} ${DEFINES} 
CC=m68k-elf-gcc
AR=m68k-elf-ar
ARFLAGS=cr

libgdb.a: $(OBJS)
	$(AR) $(ARFLAGS) libgdb.a $(OBJS) 

clean:
	rm -rf *.o *.a
