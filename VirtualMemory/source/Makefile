CC := gcc
CFLAGS := -Wall
LDFLAGS := -I$(TOP_PATH)/include

obj-y := main.o logger.o physical_memory.o

.PHONY: all clean
	
all: vm

vm: $(obj-y)
	$(CC) $(obj-y) $(CFLAGS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f *.o vm

# useful commands
# xxd -b -l 32 file --> view [file] head 32Bytes [-l 32] in binary [-b]