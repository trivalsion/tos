# @author : eltertrias
# @brief: builds x86-specific code



### Variables ###
# AS_DEFINES 		= -D ENABLE_64_BIT_BOOTSTRAP
ARCH_DIR = arch
OUTPUT_DIR_ARCH = $(BUILDDIR)/$(ARCH_DIR)
OBJECTS += $(addprefix $(OUTPUT_DIR_ARCH)/, io_x86.o init-$(ARCH).o)

AS 		= nasm
ASFLAGS	= -felf64#$(AS_DEFINES)



### Build rules ###
# Rule to make the extra stuff
all_ARCH:
	mkdir -p $(OUTPUT_DIR_ARCH)
	cp /usr/share/OVMF/OVMF_CODE.fd /usr/share/OVMF/OVMF_VARS.fd $(BUILDDIR)

$(OUTPUT_DIR_ARCH)/init-$(ARCH).o: $(ARCH_DIR)/$(ARCH)/init-x86_64.s
	$(AS) $(ASFLAGS) $^ -o $@

$(OUTPUT_DIR_ARCH)/%.o : $(ARCH_DIR)/$(ARCH)/%.c
	$(CC) $(CFLAGS) $^ -o $@

# Run with qemu-system-x86_64 with UEFI(if problems occur add this flag - "-cpu qemu64")
run_os:
	qemu-system-x86_64 \
	-drive if=pflash,format=raw,unit=0,file=$(BUILDDIR)/OVMF_CODE.fd,readonly=on \
	-drive if=pflash,format=raw,unit=1,file=$(BUILDDIR)/OVMF_VARS.fd \
	-net none \
	-drive format=raw,file=$(BUILDDIR)/$(OS_TARGET_NAME).img \
	$(QEMU_FLAGS)
	# Serial - QEMU_FLAGS+="-serial file:$(BUILDDIR)/serial.log"
	# No video  - QEMU_FLAGS+="-nographic"
	# MacOS - QEMU_FLAGS="-serial file:build/x86_64/serial.log -nographic"

.PHONY: all_ARCH run_os