ARCH   ?= x86
BUILD  := build/$(ARCH)
ISODIR := isodir
KERNEL := $(BUILD)/myos.bin
ISO    := myos-$(ARCH).iso

ifeq ($(ARCH),x86)
  CC      := i686-elf-gcc
  ASFLAGS := -f elf32
  QEMU    := qemu-system-i386
  ARCH_CFLAGS :=
  ARCH_LDFLAGS :=
else ifeq ($(ARCH),x86_64)
  CC      := x86_64-elf-gcc
  ASFLAGS := -f elf64
  QEMU    := qemu-system-x86_64
  ARCH_CFLAGS := -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2
  ARCH_LDFLAGS := -z max-page-size=0x1000
else
  $(error Unknown ARCH '$(ARCH)')
endif

AC     := nasm
CFLAGS := -std=gnu99 -ffreestanding -O2 -nostdlib -Wall -Wextra \
          -Iinclude -Iarch/$(ARCH) -MMD -MP $(ARCH_CFLAGS)

C_SRCS   := $(wildcard kernel/*.c) $(wildcard kernel/lib/*.c) $(wildcard arch/$(ARCH)/*.c)
AS_SRCS  := $(wildcard arch/$(ARCH)/*.s)
OBJS     := $(C_SRCS:%.c=$(BUILD)/%.o) $(AS_SRCS:%.s=$(BUILD)/%.o)
LDSCRIPT := arch/$(ARCH)/linker.ld

.PHONY: all clean iso run
all: $(KERNEL)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AC) $(ASFLAGS) $< -o $@

$(KERNEL): $(OBJS) $(LDSCRIPT)
ifeq ($(ARCH),x86_64)
	$(CC) -T $(LDSCRIPT) -o $(KERNEL).elf $(CFLAGS) $(ARCH_LDFLAGS) $(OBJS) -lgcc
	objcopy -O elf32-i386 $(KERNEL).elf $@
else
	$(CC) -T $(LDSCRIPT) -o $@ $(CFLAGS) $(OBJS) -lgcc
endif

iso: $(KERNEL)
	mkdir -p $(ISODIR)/boot/grub
	cp $(KERNEL) $(ISODIR)/boot/myos.bin
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

run: iso
	$(QEMU) -cdrom $(ISO) -d int,cpu_reset -D qemu.log -no-reboot

clean:
	rm -rf build $(ISODIR) *.iso qemu.log

-include $(OBJS:.o=.d)