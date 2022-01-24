## To Do
1. Change the way in which kernel determines it's location
	* Currently kernel determines it's location and size through an area markes as `kernel` in the memory map. This method is unrealiable and should not be used.
	* Fix: Remove memory map mathod from PMM and VMM code. Replace the memory map method with using linker script labels to find start, end and size of the kernel
		* [Comment about implementation](https://www.reddit.com/r/osdev/comments/oczvo8/memory_map_problems/h41xvn7/?utm_source=share&utm_medium=ios_app&utm_name=iossmf&context=3)
		* To test if the right address and size is found using labels, compare it with tomat boots results from the memory map and also try to map this address to the kernels location in the higher half instead of address from memory  map and see if everything works(if all PMM and VMM tests are passed)
2. Implement device driver interface(search for ideas on reddit and forum)
	* Implement a device detector which would use standard methods e.g. PCI or platform specific to detect all the possible devices and put the into a list of strings, when file system is implemented start drivers with these names, now just have a function that would check if some string is present and initialise the corresponding driver. Add guide to adding drivers in build system part of documentation.
3. Implement 64-Bit interrupts
	* Implement interrupt handling
	* Implement exeption handling to handle things like page faults
		* How to spot difference between exeptions and interrupts if same interrupt vectors are used
		* Map needed interrupt handlers to functions that handler the exeptions
4. Write the documentation for pmm and vmm inner working an outside interface
	* Memory management initialization proccess from PMM to VMM
	* How VMM and PMM interact
	* Description of allocation algorithms used by PMM and VMM
	* Description of VMMs and PMMs global interface functions - paste all from header
	* Description of VMM specific stuff like splitting into halves and accessing page tables(using recursive paging and identity mapping)
	* How vmm is split into source files, how vmm is configured and two types of vmms(64 and 32) and 2 parts of vmm(init part in vmm64/32.c and global vmm part with allocation in vmm.c
	* Have all of the algorithms from source copied(identity mapping, page table mapping, allocation, memory mapping)




### Improvements
* [Graphics](https://wiki.osdev.org/Category:Video)
	* Framebuffer
		* Implement cursor
		* Ability to dynamically load fonts
		* Scrolling
		* After PMM has ability to manage reserved pages, the driver should unreserve framebuffer MMIO
		* Ability to support modes with depth different from 32
		* Take the pitch into account(not currently possible to test as pith is the same as width)
		* Make sure that the framebuffer is mapped into virtual memory(above the kernel)

* [Memory Management](https://wiki.osdev.org/Category:Memory_management)
	* Interrupts should be set on hold during allocation
	* VMM
		* Make the allocation function only set the used bit(beyond the flags used by the CPU for flags, the flag should be set on a free bit that later would be used for the physical address) and not set present bit and map page. Mapping should be done by the page fault handler
		* Write Virtual Memory allocator(e.g. malloc)
	* PMM
		* In the bitmap algorithms, it should be made shure that the bitmap is mapped into the virutal memory or otherwise the OS would crash
		* Reserved memory management - there will be a linked list of bitmaps and each bitmap would correspond to each reserved area in the memory map. For drivers to request MMIO, they would specify the start address and amount of pages and if these are in the reserved range and aren't used by any other driver they would be returned. Then after driver reserves them, this memory could not be unreserved(check if the algorithm could be made easier as there is no free function) and the driver should by itself remap MMIO like framebuffers in to the programs memory space.




### [Hardware](https://wiki.osdev.org/Category:Hardware)
* [Interrupts](https://wiki.osdev.org/Category:Interrupts)
	1. [APIC(Modern)](https://wiki.osdev.org/APIC)
		* [IOAPIC](https://wiki.osdev.org/IOAPIC)
		* [APIC Timer](https://wiki.osdev.org/APIC_timer)
	* [PIC(Old)](https://wiki.osdev.org/8259_PIC)
* Time devices
	1. [HPET(Modern RTC & PIT))](https://wiki.osdev.org/HPET)
	* [PIT(Old)](https://wiki.osdev.org/Programmable_Interval_Timer)
		* Implemenent it as a device file
		* Commands from write() buffer will be sent to the PIT
		* PIT interrupts will be sent to the function that was binded to this interrupt with signal() syscall
	* [RTC(Old)](https://wiki.osdev.org/RTC)
* [ACPI](https://wiki.osdev.org/ACPI)
	* [ACPICA support](https://wiki.osdev.org/Category:ACPI)
* [ATA](https://wiki.osdev.org/Category:ATA)
	1. [AHCI(Modern SATA native way)](https://wiki.osdev.org/AHCI)
	* [PIO(Old, easier mode)](https://wiki.osdev.org/ATA_PIO_Mode)
* [USB support(PS/2, Serial, Parallel... alternative)](https://wiki.osdev.org/Category:USB)
	* [PS/2 Controller](https://wiki.osdev.org/%228042%22_PS/2_Controller)
		* Add 8042 presence check using ACPI
		* Instead of checking the status byte once and if the buffer full/empty bit is not in the right position failing, poll the status bit until time is out or it is in the right position, tracking time should be done with PIT
		* Implement PS/2 controller as a device file
			* Character device file
			* Output buffer(read with read()) will contain structures with keypress information
			* Input buffer(wrote to with write()) will send commands to the PS/2 controller
				* The message sent will first be checked to be valid(in range of possible)
* [PCI](https://wiki.osdev.org/PCI)
	* [PCI Express](https://wiki.osdev.org/PCI_Express)
	* Devices
		* [Networking](https://wiki.osdev.org/Category:Network_Hardware)
		* [Intel Sound](https://wiki.osdev.org/Intel_High_Definition_Audio)
* [Advanced integrated peripheral - alternative to all of the old chips(e.g. PIC)](https://wiki.osdev.org/Advanced_Integrated_Peripheral)
* PML5 paging instead of PML4 on x86
* [SMBIOS information parsing](https://wiki.osdev.org/System_Management_BIOS)




## Kernel
* [Memory management](https://wiki.osdev.org/Category:Memory_management)
	* [Virtual Memory Manager - mapps physical pages to virtual](https://wiki.osdev.org/Memory_Allocation)
	* [Memory Allocator - allocates space from virtual pages]()
	* [Physical Memory Manager(PMM) - allocates physical pages](https://wiki.osdev.org/Page_Frame_Allocation)
		* Add ability to manage reserved pages
* [Time Library]()
* Init system - the kernel should not have any utils(shell, coreutils) or programs in it and should call an init system which will be specified at compile time(in some config file) as an executable that will be loaded from the disk
	* A timer could be set by a programm and kernel will call the binded function when the time is up, the kernel will manage the time with whatever time device was found using the device manager(e.g. PIC)
* Boot sequence - the kernel should boot in following order - `bootloaderInterface(INIT), platformInit(), interruptsInit(), Device Detection, Init systam(that will start shell...)
* Driver functions should be not global symbols and should only be accessed through things like write and read and not symbols
* [Device manager](https://wiki.osdev.org/Device_Management)
	* [Guide](https://wiki.osdev.org/User:Kmcguire/Quick_And_Dirty_Device_Management)
	* [Thread](https://www.reddit.com/r/osdev/comments/dr2qym/device_driver_model_for_oshypervisor/)
	* There are platform essential devices that will certainly be present(like the timer) and drivers for these will be loaded automatically(not when detected, but detection could still take place if it is possible) no by enumeration. While devices located on the buses like USB or PCI should be enumerated at boot and the driver should be loaded for them
* [Multitasking](https://wiki.osdev.org/Category:Processes_and_Threads)
	* [Multitasking types](https://wiki.osdev.org/Multitasking_Systems)




## Features
* Implement the memory map cleanup algorithm
	* [Linux mmap cleanup driver(linux/arch/x86/kernel/e820.c)](https://github.com/torvalds/linux/blob/master/arch/x86/kernel/e820.c)
	* Store the memory map in a predefined safe location with size of `131 * (8 + 8 + 4)` bytes, because there could be maximum 131 entry in the map and the field sizes are 64-Bits(8 Bytes), 64-Bits(8 Bytes) and 32-Bits(4 Bytes)
* Reworked Universal Keyboard Driver
* [TTY](https://www.reddit.com/r/osdev/comments/hgzg6k/tty_questions/)
	* Be able to be used with PS/2, USB... interfaced keyboards and different video outputs
	* STDIN implementation to be read with read() syscall
	* STDOUT & STDERR to be wrote to with write() syscall
	* termios structure
	* Interchangable keymap for the keybaord with ioctl code to change it
	* Shortcuts(e.g. CTRL+C) will be implemented with signals
		* When an interrupt occurs it will be transferred to tty and the signal library will be called by it which in turn will call function binded to the interrupt(if there is one)
* [File System](https://wiki.osdev.org/Category:Filesystems)
* Unit tests for every part of the OS




## Extra
* Documentation
	* Document the algorithms that are used to manage memory in PMM(bitmap) and VMM(tree and linear algorithms)
	* Document kernel's API between different parts of the kernel:
		* PMM usage, VMM usage, interrupts usage...
	* Document the interface between kernel and device drivers and the proccess of them being loaded
	* Document kernels full boot proccess
* Switch to limine as it is maintained unlike tomatboot, but has the same protocol as tomatboot
	* Current problem: tomatboot marks only 1 memory map area as kernel area, while limine bootloader marks multiple memory map areas as kernel areas. Becuase the OSs VMM is only prepared for one memory area for kernel, multiple of these areas cause triple faults
* Create a x86_64 UEFI Bootloader
	* Use tomatboot sources stripped from multiboot, bios and other useless stuff
	* Check what other stuff should be done by the bootloader, read the UEFI manuals
	* Try to use EDK2 headers and EDK2 objects to build a UEFI app with a normal compiler and not the EDK2 build system
		* [TomatBoot does it](https://github.com/TomatOrg/TomatBoot)
	1. [GOP](https://wiki.osdev.org/GOP)
		1. Setup a framebuffer with GOP
		2. exitbootservices()
		3. Write a driver for using the raw framebuffer that was before used by UEFI
		* [GOP Thread](https://forum.osdev.org/viewtopic.php?f=1&t=26796)
	2. Exit boot services
	3. Parse the ELF headers and load OS executable
* Write a blog about making tOS for x86-64(describe all techinal info and algorithms)
* RISC(e.g. ARM, RISC-V)
	* Write a completely new, clean, small OS for ARM OR RISC-V
		* Write a blog documenting techincal information(e.g. initialization of CPU, memory, paging) and proccess of reserching and developing(include algorithms) things like vmm, driver interface and other stuff while developing an OS. Make the blog like a serious of .md files.
	* Port the tOS to ARM/RISC-V
	* Manuals
		* [ARM Manual Collection](https://community.arm.com/developer/ip-products/processors/b/processors-ip-blog/posts/important-arm-technical-learning-resources)
		* [RISC-V Manual Collection](https://gist.github.com/cb372/5f6bf16ca0682541260ae52fc11ea3bb)




## Done
- Kernel
	* [Rework Interrupts](https://wiki.osdev.org/Category:Interrupts)
		* Make a c code for interacting with interrupts(e.g. a function to set that when particular interrupt occures it should call the supplied function pointer), and the implementation of the interrupts will be seperate for arch and will be located in it's folder and then just linked with the kernel interrupts code. Architecture specific code will have all the variables for function pointers, it's own copy of the function to bind a function pointer to interrupt, the interrupts implementation - each interrupt being a function that will call a corresponding function pointer if it was set by user. Kernel wide interrupt code will contain functions for user to bind interrupt to specific function pointer, check if the interrupt exists, permissions for binding to it...
	* [Memory management](https://wiki.osdev.org/Category:Memory_management)
		* [Physical Memory Manager(PMM) - allocates physical pages](https://wiki.osdev.org/Page_Frame_Allocation)
			* Mark all memory regions reserved except the ones specified in memory map as free(no matter if region is really free or not, mem map should be followed)
			* The maximum address at which page could end should be no more then physical amount of upper RAM
			* For x86 - mark all areas below 1mb as reserved

- Hardware
	* [64bit CPU support(Long mode)](https://wiki.osdev.org/Category:X86-64)
		* Find a bootloader to use with long mode
		* [Switch to long mode](https://wiki.osdev.org/Setting_Up_Long_Mode)
		* [Setting up stack](https://forum.osdev.org/viewtopic.php?f=1&t=21772)
		* [Setup Paging](https://wiki.osdev.org/Setting_Up_Paging)
	* [UEFI firmware support(boot with UEFI services)](https://wiki.osdev.org/Category:UEFI)
		* Find a working or implement a new UEFI 64bit bootloader
		* Find a way to setup video output after exitbootservices()
	* [USB support(PS/2, Serial, Parallel... alternative)](https://wiki.osdev.org/Category:USB)
		* [PS/2 Controller](https://wiki.osdev.org/%228042%22_PS/2_Controller)
			* Implement detection of PS/2 controller presence
- ToDo
	* Implement simple memory management
		* Reimplement VMM
		* Implement uncreated functions
			* kalloc - allocated memory in higher half/kernel half
			* alloc - allocated memory in lower/user half
			* free - frees memory allocated by *alloc
			* kunmap - unmaps data by clearing the entry for page in l1 with 0
				* For safety make sure that the kmmap can’t map to an area that has present flag set, the page should be clear
		* Write Virtual Memory Manager(Page manager) and test it
	* Fix multiboot2_bootstrap(so for example the multibootsetup does not have to be commented out to run the 32 bit OS)
		* Instead make the code after the init - kernel(64-Bit code in case of x86-32) call bootloaderInterface with INIT command and it would do the needed bootloader setup based on the boot protocol
		* Make getmemorymap add entries about where multiboot_info starts and finishes - so it is not touched and where framebuffer starts and finished
	* Implement simple memory management
		* Write Page Frame Allocator
			* Should have a function to reserve physical memory based a request from device driver - special function that asks not to give it a page(4096) but to reserved all pages from start address to end address(and the ones on which start and end are located)
				* Drivers like the framebuffer driver should mark the memory mapped I/O and the memory areas that they need as reserved using PFA special function
			* Page frame allocator should not be able to touch the physical area where the kernel is, where the framebuffer is
			* The things in memory should not be to close and should have some space between them to prevent(not all but some) overflow damage
	* Implement a bootloader framebuffer driver
		* Framebuffer driver should call the bootloaderInterface with a function code to receive a standardized frambuffer struct(with fields like length width...)
		* Add this functionto documentation as it must be standardized(but it is not required for kernel to work)
		* Find a way to write the PSF file in C(temporary solution while the disk can't be read)
		* Implement functions to output font on to the screen
		* Check why the screen is yellow on boot - maybe something overwrites framebuffer

- Extra
	* Remove the 32bit mode OS, but instead have a BIOS variant of the 64bit OS(maybe by just adding another grub type in makefile)
	* Clean up the source tree
		* Change files in boot to boot protocols and not platforms
		* Rename all the same name files like init.s to init-x86_64.s
	* Documentation
		* arch_ Functions should be implemented by the platform
		* In what order should the base functions be called - bootstarp script -> kernel_setup -> kernel_init
		* Kernel requirements - `32+ Bit` Architecture, `unsigned long` in the compiler should represent largest possible var size - include/types.h has address_tt define which should be changed if compiler does not obej
		* Write how the functions with arch_ should work - for example memory map should give fields of all types
		* bootloaderInterface functions definded in init.h(BOOTLOADER_... macros) should be present