# tOS Documentation
## Authored by: eltertrias
## June 18, 2021




# Kernel Source Information
## Layout
* **arch/<arch_name>** - contains code specific to a certain CPU architecture and the linkerscript for the kernel
* **boot/<boot_system_name>** - contains the boot protocol interpreter script(in c) and bootloader config
* **drivers** - contains code for device drivers
* **include** - contains global header files for c files in the OS
	* **kcalls** - contains global headers that define the kernel calls which are like system calls but don't switch the ring 3 to ring 0(so are calls inside the kernel code to make another part of kernel do something)
	* **kernel** - contains global headers that only have use to interact between certain parts of the OS
	* **kstdlib** - contains global headers for C standard library integrated into the kernel
* **kernel** - contains the core part of OS code that manages all other parts
	* **mm** - contains code for managing both physical and virtual memory on the computer
* **lib** - contains code for libraries or parts of them that are used in the kernel(e.g. kstdlib's string.c)



## Build system
* Description - Kernels build system includes a main root file that includes all of the submakefiles into it and each of the submakefiles add objects to objects dir and later all of it is compiled
* Make Algorithm:
	* Root makefile(one in the root folder of the project):
		1. Declare global variables(e.g. CC. Variables that are needed for all submakefiles and the root makefile - global vars are declared as normal variables because any declared variable is passed onto included files). Global vars should include: $(OBJECTS) variable for holding names of compiled objects, $(CFLAGS) with "-c"(produces objects from c files that could be lnked together) as a flag and $(BUILD_DIR) to put compiled object files in.
		2. Then submakefiles should be included from projects subdirectores(include path/to/Makefile)
		3. Finally global rules should be added to root makefile and should include: all and clean(should create the build directory after deleting it). "All" rule in the root makefile should have the clean rule, "all_$(MODULE-NAME)" rules and a linking rule as depenencies in the corresponding order. The linking rule should link($(LD) $(LDFLAGS) $^(dependencies should include objects) -o exe) all the variables in the $(OBJECT) variable and have the $(OBJECTS) variable as dependency so the objects in it become dependencies and make searches for a rule to compile them and selects the one in the right makefile depending on the prefix of the object.
		4. Make sure to set the default rule of makefile and add the needed rules to .PHONY(e.g. clean, all, build)

	* Submakefiles:
		1. Declare a local build path variable $(OUTPUT_DIR_$(MODULE_NAME)) which would have the path to directory inside $(BUILD_DIR) for objects from this module(this is needed so that make calls the right compilation rule depending on the objects path) - <OUTPUT_DIR_$(MODULE_NAME) = $(BUILD_DIR)/$(MODULE_NAME)>
		2. Submakefile should add all of the objects it produces into the global $(OBJECTS) variable in a form of <OBJECTS += $(addprefix $(OUTPUT_DIR_$(MODULE_NAME))/, file.o file2.o)>
		3. Have "all_$(MODULE_NAME)" rules(MODULE_NAME is the name of the folder/module where submakefile is) which would do extra stuff that is needed before compiling objects, but they should not compile sources into objects themseleves(e.g. make a directory inside the build directory).
		4. As in %.o, % takes only the file name into account and not the prefix, rules to compile the sources should look like the one in the example and be put in every modules Makefile, this rule would only be called to compile objects that should end up in $(OUTPUT_DIR_$(MODULE_NAME)) direcotry which means the right make rule would be called for right files.
				$(OUTPUT_DIR_$(MODULE_NAME))/%.o: dir/to/sources/%.c
					$(CC) $(CFLAGS) $^ -o $@
		5. Add the needed rules to .PHONY(e.g. local all)
		6. If there are include submakefiles of subdirectories in the current makefiles subdirectory(subsubdirectories)
* Porting OS and adding parts to it
	* Add a new subfolder with a submakefile to kernel source directory structure -
		1. Create a submakefile and fill it like it is described in the "submakefiles" section of make algorithm.
		2. Place the include of submakefile near other includes in the root makefile
		3. Add the "all_<subfolder-name>" rule as a dependency to "all" rule in the root makefile right after previous "all_<subfolder-name>" rules
	* Port kernel to another architecture
		1. Create a subdirectory in "arch" called the same as architecture you are porting to
		2. Create a submakefile(follow the submakefile tutorial in section above) inside the subdirectory you created in "arch"
			* Name for local all rule should be exactly "all_ARCH"
			* Name for OS run rule should be exactly "run_os" and it should start the OS in a VM
		3. To choose the architecture for which kernel will be compiled, in the root Makefile change "ARCH" variable to the name of your architecture(which should also be the name of subdirectory you created in "arch" direcotry)
	* Add a new boot system
		1. Create a subdirectory in "boot" called the same as the bootloader you are adding
		2. Create a submakefile(follow the submakefile tutorial in section above) inside the subdirectory you created in "boot"
			* Name for local all rule should be exactly "all_BOOT_SYSTEM"
			* Name for rule to build a media from which os is started should be exactly "make_boot_media" and should make a bootable media out of the kernel executable
		3. To choose the bootloader with which kernel will be run, in the root Makefile change "BOOT_SYSTEM" variable to the name of your bootloader(which should also be the name of subdirectory you created in "boot" direcotry)
	* Add a source file to the kernel
		1. Create the source file in the chosen subdirectory and add it to the <OBJECTS += $(addprefix $(BUILDDIR)/$(DIR)/, file.o)> variable assignment as addprefix argument(the file name you add should be - file_name.o)
	* Create a subdirectory in a subdirectory
		* Local submakefile - should be included in submakefiles in a directory before them and should not have a local "all rule". So this kind of submakefile should only add objects to the objects var and have a compile rule(dir/%.o : dir/%.c), the build directory for it should be created in "all rule" of makefile in previous directory
* Compiling on OSs other than Linux
	* MacOS - there is not way to compile on MacOS, so linux VM should be used to compile and start tOS on MacOS
		1. Create a Linux VM(Fedora Server(No GUI) or Ubuntu Server(No GUI) is recommended) that could be accessed through SSH and add a shared directory to it(shared directory on host should be github/tos and not github/tos/src)
			* [Guide](https://github.com/trivalsion/unix/blob/master/config/misc/macos-vbox-linux-setup.md)
		2. Edit code on MacOS(as code should be located on MacOS and accessed by linux VM using the shared directory)
		3. Create a new SSH terminal or use already created one to access Linux VM through SSH
		4. On Linux VM - Go to /shared/directory/src
		5. On Linux VM - Run `make` with the following options `QEMU_FLAGS="-serial file:build/x86_64/serial.log -nographic"` - `make QEMU_FLAGS="-serial file:build/x86_64/serial.log -nographic"`
		* If something is slow increase amount of ram and/or amount of CPU cores



# CPU/Architecture requirements for proting
* CPU should be 32 or 64 Bit
* CPU should contain an MMU
* CPU should be little-endian
* At least 512 KiB of usable RAM should be available
* Compiler "unsigned long" and "size_t" types in C should be "address-width" size




# Bootloader Requirements
## Introduction
This section described how the OS could be ported to another bootloader or CPU architecture. **To interface with different boot protocols the OS requires a special bootstrap script that would setup everything and provide the information to the kernel in the form of a structure.**



## OS interface for the boot protocol
To interface with bootloaders that use different boot protocol OS uses an abstraction layer called bootstarp script that should setup the environment for the OS to use and provide kernel information in form of a data structure



## Bootstrap script functions
### Function to start the transition to OS code
```C
tosBootProtocol_tt arch_bootloader(void)
```
- **Return** - returns the structure with boot information(structure format specified in include/kernel/init.h)
- **The function should:**
	* Fill all of the structures and fields in the boot information structure
	* Set the present flag in all of sub-structs and fields. The present flag macro is specific for each sub-struct and should be set in it's flag variable


### Function that starts the bootloader interpreter
```C
void _start()
```
- **The function should:**
	* Start the OS by calling bootloader interpreter
	* Should NOT return back to it's caller in any case



## Boot process
* First the bootloader should call **_start** function which should be implemented by bootstarp scipt and should by itself or by calling other functions do necessary setup for the OS and read bootloader supplied data and provide it to the OS using boot information structure. **_start** function should setup a stack of size that is no less then 16KB.
* Next **_start** should call the **kernel_setup** function
* **kernel_setup** function should do(if needed) platform specific setup(mostly using assembly code) and then call **kernel_main** function to start the OS