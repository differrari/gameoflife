ARCH       ?= aarch64-none-elf
CC         := $(ARCH)-gcc
CXX        := $(ARCH)-g++
LD         := $(ARCH)-ld


# Path to the system's libshared. Static link only for now
STDINC ?= ../os/shared/
STDLIB ?= ../os/shared/libshared.a
CFLAGS ?= -ffreestanding -nostdlib -std=c99 -I$(STDINC) -O0
OUT ?= god.elf
FS_PATH ?= ../os/fs/redos/user/$(OUT)

.PHONY: dump

all:
	$(CC) $(CFLAGS) -c god.c -o god.o
	$(LD) -T linker.ld -o $(OUT) god.o $(STDLIB)

run: all
	cp $(OUT) $(FS_PATH)
	make -C ../os run

clean: 	
	rm god.o
	rm $(OUT)

dump:
	$(ARCH)-objdump -D god.elf > dump