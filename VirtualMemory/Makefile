CC := gcc
TOP_PATH := $(shell pwd)

export CC TOP_PATH CFLAGS LDFLAGS

.PHONY: all clean

all:
	$(MAKE) -C $(TOP_PATH)/source
	@cp $(TOP_PATH)/source/vm $(TOP_PATH) -f

clean:
	$(MAKE) -C $(TOP_PATH)/source clean
	@cp -f /tmp/*file.txt /tmp/*file.bin $(TOP_PATH)
	@rm -f vm /tmp/*file.txt /tmp/*file.bin