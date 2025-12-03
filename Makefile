ARCH       ?= aarch64-none-elf-
CC         := $(ARCH)gcc
CXX        := $(ARCH)g++
LD         := $(ARCH)ld
DUMP	   := $(ARCH)dump

STDINC ?= ../os/shared/
STDLIB ?= ../os/shared/libshared.a
CFLAGS ?= -std=c99 -I$(STDINC) -O0 -I$(INCLUDES)
OUT ?= god.elf
FS_PATH ?= ../os/fs/redos/user/$(OUT)

ifeq ($(ARCH), aarch64-none-elf-)
	CFLAGS += -nostdlib -ffreestanding 
else 

endif

.PHONY: dump

all:
	$(CC) $(LFLAGS) $(CFLAGS) $(shell find . -name '*.c') -I$(STDINC) $(CROSSLIB) ../os/shared/libshared.a -o $(OUT) --verbose
	chmod +x $(OUT)

run: all
ifeq ($(ARCH), aarch64-none-elf-)
	cp $(OUT) $(FS_PATH)
	make -C ../os run
else
	./$(OUT)
endif

clean: 	
	rm $(OUT)
	rm $(shell find . -name '*.o')

cross:
	$(MAKE) ARCH= INCLUDES=../cross CROSSLIB=../cross/crosslib.a
	./$(OUT)

dump:
	$(DUMP) -D $(OUT) > dump