# Makefile
# David Grant, 2004
# Irene Huang, 2010/05/03 
# Mikhail Zakharov, 2010/05/11

#where all the headders are
INC=-Icommon/

#define your ifdef flags here
DEFINES+=-D_DEBUG_HOTKEYS
#DEFINES+=-D_CFSERVER_
#TEST=OFF

#optimization level
#OPT=-Os

SYSTEM=$(shell uname)

ifneq ($(EMULATOR),OFF)
DEFINES+=-D_EMULATOR_
endif


TEST_PHONY=-D_TEST_
ifeq ($(TEST),OFF)
TEST_PHONY=-D_DO_NOT_TEST_
endif

ifeq ($(SYSTEM),Linux)
KILLER=killall
else
KILLER=taskkill /F /IM 
endif

ifeq ($(TEST_TARGET), internal)
RTX_TEST=rtx_test_int
RTX_TEST_OBJS+=rtx_test_int.o random_gen.o
EXT_PHONY=-D_INTERNAL_
else
RTX_TEST=rtx_test_ext
RTX_TEST_OBJS+=rtx_test/rtx_test_ext.o rtx_test/basic_message.o rtx_test/basic_priority.o \
		rtx_test/basic_mem.o rtx_test/basic_release_processor.o  \
		rtx_test/advanced_memory_schedueling.o
endif


ifeq ($(GDB_SESSION),ON)
DEFINES+=-D_GDB_
INC+=-Igdb/
LIBGDB=-static -Lgdb -lgdb 
endif

CFLAGS=-Wall $(OPT) -g -Werror -m5307 -pipe -nostdlib ${INC} ${DEFINES} ${ECLIPSE}\
	 ${TEST_PHONY} ${EXT_PHONY}


CC=m68k-elf-gcc -g
COLDFIRE_EMU=coldfire
ifneq ($(SYSTEM),Linux)
COLDFIRE_EMU=coldfire.exe
endif
BOARD=--board cjdesign-5307.board
CXX=m68k-elf-g++
LD=m68k-elf-gcc
AS=m68k-elf-as
GDB=m68k-elf-gdb
ifneq ($(SYSTEM),Linux)
GDB=m68k-elf-gdb.exe
endif
AR=m68k-elf-ar
ARFLAGS=
OBJCPY=m68k-elf-objcopy
OBJDMP=m68k-elf-objdump
ASM=common/start.s
LDFLAGS+=-Tcommon/rtx.ld -Wl,-Map=main.map 
MAIN_OBJS+=dbug/dbug.o dbug/printf.o utils.o kernel/rtx_api.c \
 kernel/kernel.o kernel/api_entry.o kernel/memory.o  kernel/process.o kernel/priority_queue.o\
 kernel/i_process.o kernel/i_entry.o sys_process.o user_process.o rtx_queue.o

RTX_TEST_OBJS += dbug/dbug.o dbug/printf.o
LDFLAGS_RTX_TEST = -Tcommon/rtx_test.ld -Wl,-Map=rtx_test.map

LDFLAGS_RTX_LOADER = -Tcommon/rtx_loader.ld -Wl,-Map=rtx_loader.map
RTX_LOADER_OBJS = dbug/dbug.o rtx_loader.o

ext: 
	make TEST_TARGET=external merge.s19
int:
	make TEST_TARGET=internal merge.s19
# Note, GCC builds things in order, it's important to put yhe
#  ASM first, so that it is located at the beginning of our program.
main.s19: $(MAIN_OBJS) main.c
	$(CC)  $(CFLAGS) $(LDFLAGS) -o main.bin  $(ASM) main.c $(LIBGDB) $(MAIN_OBJS) 
	$(OBJCPY) --output-format=srec main.bin main.s19
	$(OBJDMP) -xdC main.bin > main.lst 
	
$(RTX_TEST).s19: $(RTX_TEST_OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS_RTX_TEST) -o $(RTX_TEST).bin $(ASM) $(RTX_TEST_OBJS) 
	$(OBJCPY) --output-format=srec $(RTX_TEST).bin $(RTX_TEST).s19
	$(OBJDMP) -xdC $(RTX_TEST).bin > $(RTX_TEST).lst

rtx_loader.s19: $(RTX_LOADER_OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS_RTX_LOADER) -o rtx_loader.bin $(ASM) $(RTX_LOADER_OBJS) 
	$(OBJCPY) --output-format=srec rtx_loader.bin rtx_loader.s19
	$(OBJDMP) -xdC rtx_loader.bin > rtx_loader.lst

libgdb:
	cd gdb && make

mdummy.s19: rtx_loader.s19 main.s19
	python merge.py mdummy.s19 rtx_loader.s19 main.s19
	chmod u+x mdummy.s19
	
merge.s19: mdummy.s19 $(RTX_TEST).s19 	
	python merge.py merge.s19 mdummy.s19 $(RTX_TEST).s19 
	chmod u+x merge.s19
	
mdummy2.s19: rtx_loader.s19 rtx.s19
	python merge.py mdummy2.s19 rtx_loader.s19 rtx.s19
	chmod u+x mdummy2.s19
	
extos: mdummy2.s19 $(RTX_TEST).s19 	
	python merge.py merge.s19 mdummy2.s19 $(RTX_TEST).s19 
	chmod u+x merge.s19	

exttest: mdummy.s19 alien.s19 	
	python merge.py merge.s19 mdummy.s19 alien.s19 
	chmod u+x merge.s19
	
rtx:
	make clean
	make TEST=OFF EMULATOR=OFF main.s19 
rtx_tester:
	make clean
	make TEST=ON EMULATOR=OFF TEST_TARGET=external merge.s19

clean:
	rm -f *.s19 *.o *.orig *.bin *.lst *.map rtx_test/*.o proc_switch/*.o kernel/*.o gdb/*.o dbug/*.o

#starts console gdb session
gdbe:
	$(GDB) main.bin --x gdbcommand

#kills all coldfire processes on windows
kill:
	-$(KILLER) $(COLDFIRE_EMU)
	-$(KILLER) $(GDB)

#use this to compile with the gdb stub support
debug:
	rm -rf main.s19 kernel/i_process.o
	make GDB_SESSION=ON TEST_TARGET=external merge.s19
	rm -rf main.s19 kernel/i_process.o


#starts the emulator, downloads the main binary, and executes
emu:
	${COLDFIRE_EMU} ${BOARD} --commands "dl merge.s19,go 10200000"
cold:
	${COLDFIRE_EMU} ${BOARD}

#generate tags for vim
tag:
	ctags -R --language-force=c .

dos2unix:
	find . | awk '{print "\""$$1"\""}' | xargs dos2unix
