# @author : eltertrias
# @brief : builds files for interpreting stivale2 boot protocol



### Variables ###
BOOT_DIR = boot
OUTPUT_DIR_BOOT_SYSTEM = $(BUILDDIR)/$(BOOT_DIR)
OBJECTS += $(addprefix $(BUILDDIR)/$(BOOT_DIR)/, stivale.o)


### Build rules ###
all_BOOT_SYSTEM:
	mkdir -p $(OUTPUT_DIR_BOOT_SYSTEM)

$(OUTPUT_DIR_BOOT_SYSTEM)/%.o: $(BOOT_DIR)/$(BOOT_SYSTEM)/%.c
	$(CC) $(CFLAGS) $^ -o $@

# Build a FAT32 disk with the UEFI bootloader and the kernel on it ##
make_boot_media:
	# Compile the bootloader and put into the build dir
	make -C ../tomatboot
	cp ../tomatboot/bin/BOOTX64.EFI $(BUILDDIR)

	# Create an empty disk image with size of 94 KB
	dd if=/dev/zero of=$(BUILDDIR)/$(OS_TARGET_NAME).img bs=512 count=500000

	# Format the disk image with 1440 byte sectors, if problems occur add -f 1440
	mformat -i $(BUILDDIR)/$(OS_TARGET_NAME).img ::

	# Create EFI and then EFI/BOOT directory on the disk image
	mmd -i $(BUILDDIR)/$(OS_TARGET_NAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OS_TARGET_NAME).img ::/EFI/BOOT

	# Copy the efi bootloader to the EFI/BOOT folder on the drive
	mcopy -i $(BUILDDIR)/$(OS_TARGET_NAME).img $(BUILDDIR)/BOOTX64.EFI ::/EFI/BOOT

	# Copy the kernel/executable to the drive
	mcopy -i $(BUILDDIR)/$(OS_TARGET_NAME).img $(BUILDDIR)/$(OS_TARGET_NAME).o ::/EFI/BOOT

	# Copy bookloade into img file
	mcopy -i $(BUILDDIR)/$(OS_TARGET_NAME).img boot/$(BOOT_SYSTEM)/limine.cfg ::

.PHONY: all_BOOT_SYSTEM make_boot_media