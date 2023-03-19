AS      := x86_64-elf-as
CC      := x86_64-elf-gcc
LD      := x86_64-elf-gcc
OBJCOPY := x86_64-elf-objcopy

KERNEL_VMA_BASE    := 0xffffffff80000000
KERNEL_ASPACE_BASE := 0xffffff8000000000
KERNEL_ASPACE_SIZE := 0x0000008000000000

BUILD_DIR     := build
BUILD_DIR_OBJ := build/obj

# CFLAGS := -Wall \
		-Wextra \
		-nostdlib \
		-nostartfiles \
		-mcmodel=large \
		-mno-red-zone \
		-fno-builtin \
		-fno-pic \
		-fno-stack-protector \
		-fno-common \
		-fasynchronous-unwind-tables \

CFLAGS := -mcmodel=large -mno-red-zone -fno-stack-protector -fno-builtin
CFLAGS += -I.
CFLAGS += -DKERNEL_VMA_BASE=$(KERNEL_VMA_BASE)
CFLAGS += -DKERNEL_ASPACE_BASE=$(KERNEL_ASPACE_BASE)
CFLAGS += -DKERNEL_ASPACE_SIZE=$(KERNEL_ASPACE_SIZE)
# CFLAGS += -g -gdwarf-2 -O0

LDFLAGS := -z max-page-size=4096 -T $(BUILD_DIR_OBJ)/kernel.generated.lds -n -nostdlib -mno-red-zone -flto

all: kernel

%.o: %.S
	$(CC) -c $< $(CFLAGS) -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

kernel: clean
	mkdir -p $(BUILD_DIR_OBJ)/
	$(CC) -c $(CFLAGS) mmu.c -o $(BUILD_DIR_OBJ)/mmu.o
	$(CC) -c $(CFLAGS) console.c -o $(BUILD_DIR_OBJ)/console.o

	$(CC) -c $(CFLAGS) printf.c -o $(BUILD_DIR_OBJ)/printf.o

	$(CC) -c $(CFLAGS) cpu_data.c -o $(BUILD_DIR_OBJ)/cpu_data.o
	$(CC) -c $(CFLAGS) thread.c -o $(BUILD_DIR_OBJ)/thread.o

	$(CC) -c $(CFLAGS) pmm.c -o $(BUILD_DIR_OBJ)/pmm.o
	$(CC) -c $(CFLAGS) vmm.c -o $(BUILD_DIR_OBJ)/vmm.o
	$(CC) -c $(CFLAGS) vm.c -o $(BUILD_DIR_OBJ)/vm.o
	$(CC) -c $(CFLAGS) balloc.c -o $(BUILD_DIR_OBJ)/balloc.o
	$(CC) -c $(CFLAGS) string.c -o $(BUILD_DIR_OBJ)/string.o
	$(CC) -c $(CFLAGS) kheap.c -o $(BUILD_DIR_OBJ)/kheap.o
	$(CC) -c $(CFLAGS) pgalloc.c -o $(BUILD_DIR_OBJ)/pgalloc.o

	$(CC) -c $(CFLAGS) console.c -o $(BUILD_DIR_OBJ)/console.o
	$(CC) -c $(CFLAGS) interrupt.c -o $(BUILD_DIR_OBJ)/interrupt.o
	$(CC) -c $(CFLAGS) irq.c -o $(BUILD_DIR_OBJ)/irq.o
	$(CC) -c $(CFLAGS) debug.c -o $(BUILD_DIR_OBJ)/debug.o
	$(CC) -c $(CFLAGS) platform.c -o $(BUILD_DIR_OBJ)/platform.o
	$(CC) -c $(CFLAGS) arch.c -o $(BUILD_DIR_OBJ)/arch.o
	$(CC) -c $(CFLAGS) qsort.c -o $(BUILD_DIR_OBJ)/qsort.o
	$(CC) -c $(CFLAGS) init.c -o $(BUILD_DIR_OBJ)/init.o
	$(CC) -c $(CFLAGS) main.c -o $(BUILD_DIR_OBJ)/main.o

	$(CC) -c $(CFLAGS) -D__ASSEMBLY__ exception.S -o $(BUILD_DIR_OBJ)/exception.o
	$(CC) -c $(CFLAGS) -D__ASSEMBLY__ start.S -o $(BUILD_DIR_OBJ)/start.o

	$(CC) -E -I. -D__ASSEMBLY__ -P kernel.lds.S -o $(BUILD_DIR_OBJ)/kernel.generated.lds
	
	$(LD) -r \
		$(BUILD_DIR_OBJ)/start.o $(BUILD_DIR_OBJ)/arch.o $(BUILD_DIR_OBJ)/mmu.o $(BUILD_DIR_OBJ)/string.o \
		$(BUILD_DIR_OBJ)/exception.o $(BUILD_DIR_OBJ)/interrupt.o $(BUILD_DIR_OBJ)/balloc.o $(BUILD_DIR_OBJ)/printf.o \
		$(BUILD_DIR_OBJ)/console.o $(BUILD_DIR_OBJ)/debug.o $(BUILD_DIR_OBJ)/platform.o $(BUILD_DIR_OBJ)/pmm.o \
		$(BUILD_DIR_OBJ)/kheap.o $(BUILD_DIR_OBJ)/pgalloc.o $(BUILD_DIR_OBJ)/irq.o $(BUILD_DIR_OBJ)/qsort.o \
		$(BUILD_DIR_OBJ)/vmm.o $(BUILD_DIR_OBJ)/vm.o $(BUILD_DIR_OBJ)/main.o $(BUILD_DIR_OBJ)/init.o  \
		$(BUILD_DIR_OBJ)/cpu_data.o $(BUILD_DIR_OBJ)/thread.o \
		-o $(BUILD_DIR_OBJ)/kernel.o

	$(LD) $(LDFLAGS) $(BUILD_DIR_OBJ)/kernel.o -o $(BUILD_DIR)/rix.elf

	cp $(BUILD_DIR)/rix.elf isodir/boot/rix.elf
	grub-mkrescue isodir -o $(BUILD_DIR)/rix.iso

qemu_monitor:
	qemu-system-x86_64 --cdrom $(BUILD_DIR)/rix.iso --monitor telnet:127.0.0.1:1234,server,nowait

qemu:
	qemu-system-x86_64 --cdrom $(BUILD_DIR)/rix.iso -s -m 6G

clean:
	rm -rf $(BUILD_DIR)/
