QEMU := qemu-system-riscv32
CC := /usr/bin/clang
OBJCOPY := /usr/bin/llvm-objcopy
BUILD_DIR := build

CFLAGS := -std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf \
          -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib

KERNEL_SRCS := kernel.c memory.c sbi.c virtio_blk.c fs.c process.c trap.c common.c
SHELL_SRCS := shell.c user.c common.c

.PHONY: all run clean

all: $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/disk.tar

run: all
	$(QEMU) -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
	    -d unimp,guest_errors,int,cpu_reset -D $(BUILD_DIR)/qemu.log \
	    -drive id=drive0,file=$(BUILD_DIR)/disk.tar,format=raw,if=none \
	    -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
	    -kernel $(BUILD_DIR)/kernel.elf

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_SRCS) kernel.h common.h kernel.ld $(BUILD_DIR)/shell.bin.o Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Wl,-Tkernel.ld -Wl,-Map=$(BUILD_DIR)/kernel.map -o $@ \
	    $(KERNEL_SRCS) $(BUILD_DIR)/shell.bin.o

$(BUILD_DIR)/shell.elf: $(SHELL_SRCS) user.h common.h user.ld Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Wl,-Tuser.ld -Wl,-Map=$(BUILD_DIR)/shell.map -o $@ $(SHELL_SRCS)

$(BUILD_DIR)/shell.bin: $(BUILD_DIR)/shell.elf Makefile
	$(OBJCOPY) --set-section-flags .bss=alloc,contents -O binary $< $@

$(BUILD_DIR)/shell.bin.o: $(BUILD_DIR)/shell.bin Makefile
	$(OBJCOPY) -Ibinary -Oelf32-littleriscv \
	    --redefine-sym _binary_build_shell_bin_start=_binary_shell_bin_start \
	    --redefine-sym _binary_build_shell_bin_end=_binary_shell_bin_end \
	    --redefine-sym _binary_build_shell_bin_size=_binary_shell_bin_size \
	    $< $@

$(BUILD_DIR)/disk.tar: disk/*.txt Makefile | $(BUILD_DIR)
	cd disk && tar cf ../$@ --format=ustar *.txt

clean:
	rm -rf $(BUILD_DIR)
