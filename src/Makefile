# @author : eltertrias
# @brief : the main Makefile for tos
# @notes :
#	* Run on real hardware: The image file created by [disk_x86_64-uefi] can be directly written to usb drive with:
# 		* sudo dd if=build/x86_64/tos.img of=/dev/sdX
# 		* SECURE BOOT SHOULD BE TURNED OFF IN BIOS
#		* IF SOMETHING FAILS - the tos.img is probably compressed and should be uncompressed. uncompressed img is same as iso, but compressed can't be booted directly
#	* Make system idea - have variables with lists of objects and the submakefiles will add object names with the directory prefix(attached to the object name so it's subdir could be determined), and in each submakefile there will be rules like "prefix/%.o : prefix/%.c" and as the list of all objects would be in the main root Makefile as dependencies to build rule, make would use "prefix/%.o : prefix/%.c" rules depending on the prefix
# Make sure to start make with - make --warn-undefined-variables



### Variables ###
## tOS config vars
ARCH = x86_64# Default: x86_64, Others: None(arch variable can be set when calling make)
ARCH_TARGET = $(ARCH)-elf
COMPILER = CLANG# Default: x86_64, Others available: GCC
BOOT_SYSTEM = stivale2

BUILDDIR = build/$(ARCH)
INIT_SOURCES = arch/$(ARCH)
TOOLCHAIN_PREFIX = ../toolchain/bin
OBJECTS =
OS_TARGET_NAME = tos


## C build vars
CFLAGS_GLOBAL			= -Wmissing-prototypes -Wshadow -Wall -Wextra -c
CFLAGS_EXTRA_WARNINGS 	= -pedantic -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused


## GCC build vars
CC_GCC = $(TOOLCHAIN_PREFIX)/x86_64-elf-gcc	# if problems occur, use gcc
LD_GCC = $(TOOLCHAIN_PREFIX)/x86_64-elf-gcc	# if problems occur, use gcc
CFLAGS_GCC 	= -I include -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 $(CFLAGS_GLOBAL)	# (-ffreestanding) Startup not at main() and no standard lib; (-mcmodel=large) No assumptions about addresses; (-mno-red-zone -mno-mmx -mno-sse -mno-sse2) Disable CPU specific features that could generate runtime errors
LDFLAGS_GCC 	= -T $(INIT_SOURCES)/link-$(ARCH).ld -ffreestanding -nostdlib -l gcc											# (-T script) Use custom linker script; (-ffreestanding)Startup not at main() and no standard lib; (-nostdlib) Do not link with standard library; (-l gcc) Link with gcc library


## Clang build vars
CC_CLANG = clang
LD_CLANG = ld.lld # if problems occur, change to clang
CFLAGS_CLANG		= -I include -ffreestanding -target $(ARCH_TARGET) -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 $(CFLAGS_GLOBAL)	# (-ffreestanding, -nostdlib) Startup not at main() and no standard lib; (-mno-*) disable different compiler features; (-mcmodel=large) No assumptions about addresses; (-target) specify target arch; (-fuse-lld) choose alternative linker
LDFLAGS_CLANG	= -T $(INIT_SOURCES)/link-$(ARCH).ld -nostdlib # if problems occur, add to flags - -ffreestanding -fuse-ld=lld
ifdef LD_LLD			# IF THE KERNEL LINKS INCORRECTLY OR CLANG SAYS IT WILL IGNORE -T
LD_CLANG = ld.lld
LDFLAGS_CLANG 		= -T $(INIT_SOURCES)/link-$(ARCH).ld -nostdlib
endif


## Selected build vars
CC 		= $(CC_$(COMPILER))
LD 		= $(LD_$(COMPILER))
CFLAGS 	= $(CFLAGS_$(COMPILER))
LDFLAGS = $(LDFLAGS_$(COMPILER))



### Submakefile includes ###
include boot/$(BOOT_SYSTEM)/Makefile
include $(INIT_SOURCES)/Makefile
include lib/Makefile
include kernel/Makefile



### Build rules ###
## Compilation rules
all: clean all_ARCH all_BOOT_SYSTEM all_LIBS all_KERNEL build make_boot_media run_os

build: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/$(OS_TARGET_NAME).o


## Special rules
# Toolchain Build
toolchain:
	read -p "Are you sure you want to compile the toolchain(Press any button to continue): " toolchain_compile
	chmod +x make/toolchain.sh
	make/toolchain.sh

# Build clean up
clean:
	rm -rf *.o *.so *.out *.img *.fd *.efi *iso $(BUILDDIR)
	mkdir -p $(BUILDDIR)



### Reserved variables and rules ###
.PHONY: toolchain clean all build
.DEFAULT_GOAL = all