CC		= i686-elf-gcc
AC		= nasm
CFLAGS	= -ffreestanding -O2 -nostdlib -Iinclude
ASFLAGS = -f elf32

BUILD 	= build
ISODIR	= isodir
C_DIR 	= src
AS_DIR	= assembly

C_OBJS	= $(BUILD)/kernel.o $(BUILD)/descriptors.o $(BUILD)/vga.o $(BUILD)/handlers.o $(BUILD)/pic.o $(BUILD)/console.o $(BUILD)/pmm.o $(BUILD)/vmm.o
OBJS	= $(BUILD)/boot.o $(BUILD)/tables_asm.o $(BUILD)/virtual_memory.o $(C_OBJS)
KERNEL	= $(BUILD)/myos.bin
ISO		= myos.iso

.PHONY: all clean iso run

all: $(KERNEL)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(AS_DIR)/%.s | $(BUILD)
	$(AC) $(ASFLAGS) $< -o $@

$(BUILD)/%.o: $(C_DIR)/%.c | $(BUILD)
	$(CC) -c $< -o $@ $(CFLAGS) -std=gnu99 -Wall -Wextra

$(KERNEL): $(OBJS) linker.ld
	$(CC) -T linker.ld -o $(KERNEL) $(CFLAGS) $(OBJS) -lgcc

iso: $(KERNEL)
	mkdir -p $(ISODIR)/boot/grub
	cp $(KERNEL) $(ISODIR)/boot/myos.bin
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD) $(ISODIR) $(ISO)