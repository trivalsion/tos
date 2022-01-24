# OS Development Resources(code, books, websites...)
## Books
* Practical
	* **MMURTL V1.0** - Richard Burgess `Best`
	* **OS Design: The XINU Approach** - Douglas Comer `Best`
	* **xv6 Book(RISC-V and x86)** - MIT
	* **Little OS Book** - Erik Helin

* Theory
	* **OSs: Design and Implementation(Ed. 3)** - Andrew Tanenbaum `Best`
	* **Operating Systems: 3 Easy pieces** - Remzi Arpaci-Dusseau `Best`
	* **Modern OSs** - Andrew Tanenbaum
	* **OS Concepts** - Avi Silberschatz
	* **Operating Systems: Internals and Design Principles** - W. Stallings

* Existing OS Theory
	* **Design of a UNIX OS** - Maurice Bach `Best`
	* **Lion's Commentary on UNIX** - John Lions
	* **Design and Implementation of FreeBSD(Ed. 2)** - Marshall McKusick
	* **Windows Internals(Ed. 7)(Part 1 and 2)** - Mark Russinovich

* Linux Kernel
	* **Linux Kernel Development(Ed. 3)** - Robert Love
	* **How Linux Works: What Every Superuser Should Know(Ed. 2)** - Brian Ward
	* **Understanding the Linux Kernel(Ed. 3)** - D. Bovet
	* **A Heavily Commented Linux Kernel 0.12 Code** - Zhao Jiong
	* **Linux Kernel in a Nutshell** - Greg Kroah-Hartman

* Driver Development
	* **Linux Device Drivers(Ed. 3)** - Greg Kroah-Hartman
		* [Drivers from the book don't compile on new kernels, so this projects aims to update them for new kernels](https://github.com/martinezjavier/ldd3)
	* **FreeBSD Device Drivers** - Jospeh Kong

* Tools
	* **C Primer Plus(C tutorial)** - Stephen Prata

* Book collections
	* [OSDev.org books list](https://wiki.osdev.org/Books)



## Websites
* [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
* [LowLeve](https://www.lowlevel.eu/wiki/Hauptseite)



## Specifications
* CPU
	* [Intel Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
		* [Intel 64 & IA-32 Programmers Manual(All Volumes)](https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html)
		* [Intel i386(80386)(1986) Programmers Manual](https://css.csail.mit.edu/6.858/2014/readings/i386.pdf)
	* [AMD64 Manuals](https://developer.amd.com/resources/developer-guides-manuals/)
		* [AMD64 Programmer's Manual(All Volumes)](https://www.amd.com/system/files/TechDocs/40332.pdf)
* Boot
	* [UEFI Specifications](https://www.uefi.org/specifications)
	* [BIOS Specification](https://www.scs.stanford.edu/nyu/04fa/lab/specsbbs101.pdf)
	* [Multiboot 2 Specification(Used by GRUB)](https://www.gnu.org/software/grub/manual/multiboot2/)
	* [Stivale Boot Specification and Headers](https://github.com/stivale/stivale)
		* [Stivale2 Spec](https://github.com/stivale/stivale/blob/master/STIVALE2.md)
		* [Stivale2 Example Kernel](https://github.com/limine-bootloader/limine/tree/trunk/test)
		* [TomatBoot Spec](https://github.com/TomatOrg/TomatBoot/blob/master/README.md)
* Programming
	* [NASM Documentation](https://www.nasm.us/docs.php)
	* [Clang Documentation](https://clang.llvm.org/docs/index.html)
		* [Cross-compilation Info](https://clang.llvm.org/docs/CrossCompilation.html)
		* [Assembling Toolchain](https://clang.llvm.org/docs/Toolchain.html)
	* [LLD Documentation](https://lld.llvm.org/)
	* [GNU Binutils(ld)](https://www.gnu.org/software/binutils/docs-2.36/)
		* [Linkerscript guide](https://sourceware.org/binutils/docs/ld/Scripts.html)
	* [Make Documentation](https://www.gnu.org/software/make/manual/)
		* [Usage Guide](https://makefiletutorial.com/)
	* System V Spec
		* [Sys V AMD64 ABI](https://gitlab.com/x86-psABIs/x86-64-ABI)
		* [Links](https://wiki.osdev.org/System_V_ABI#Documents)
* Implementation
	* [Linux Kernel Page Table Management](https://www.kernel.org/doc/gorman/html/understand/understand006.html)



## Courses
* Tutorials
	* [OSDev Wiki - Writing an Operating system](https://wiki.osdev.org/Creating_an_Operating_System)
	* [LowLevel - OSDev Website](http://www.lowlevel.eu/wiki/Lowlevel:Portal)
	* [Bham OS Book](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
	* [Bona Fide OSdev](http://www.osdever.net/tutorials/)
	* [Bran's KernelDev Tutorial](http://www.osdever.net/bkerndev/index.php)
	* [James KernelDev Guide](http://www.jamesmolloy.co.uk/tutorial_html/)
	* [BrokenThron OS Tutorials](http://www.brokenthorn.com/Resources/OSDevIndex.html)
	* [WhyOS Lectures and Tutorials](http://wyoos.org/index.php)
	* [UEFI From Scratch](https://youtu.be/01-UA2LKQ9Y)
	* [OS Lectures and Classes](https://ops-class.org/)
	* [Building a 64bit bootloader](https://github.com/gmarino2048/64bit-os-tutorial)
	* [Building a x86_64 OS](https://ethv.net/workshops/osdev/notes/notes-0.html)

* University Courses and lectures
	* [MIT 6.828 Course](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-828-operating-system-engineering-fall-2012/index.htm)
	* [MIT 6.828 Course Practical](https://pdos.csail.mit.edu/6.828/2020/schedule.html)
	* [MIT 6.828 Course xv6 Practical](https://pdos.csail.mit.edu/6.828/2012/)
	* [MIT 6.828 Course Tasks](https://pdos.csail.mit.edu/6.828/2016/homework/)
	* [Wisconsin University OS Lectures](http://pages.cs.wisc.edu/~bart/537/lecturenotes/titlepage.html)
	* [Harward Course PintOS Practical](https://web.stanford.edu/class/cs140/projects/pintos/pintos_1.html)
	* [IITD OS Lectures](https://iitd-plos.github.io/os-nptel/)


* Not standard OS courses
	* [RPi OS Tutorial](https://jsandler18.github.io/)
	* [Rust RISC-V OS](https://osblog.stephenmarz.com/index.html)
	* [Rust x86 OS](https://os.phil-opp.com/)



## Source Code
* Professional OSs
	* [UNIX(All versions)(10k)](https://minnie.tuhs.org/cgi-bin/utree.pl)
	* [UNIX Development from 1970 to today(each version is in a seperate branch)](https://github.com/dspinellis/unix-history-repo)
	* [Minix 1,2(50k)](https://wiki.minix3.org/doku.php?id=www:download:previousversions)
	* [Linux 0.01-0.99(10k)](https://mirrors.edge.kernel.org/pub/linux/kernel/Historic/)
		* [All linux versions source navigator](https://elixir.bootlin.com/linux/0.11/source)
	* [LittleKernel - ARM/Android proffessional kernel/bootloader(700k)](https://github.com/littlekernel/lk)
	* [FreeBSD - Clean, large code; lots of docs](https://freebsd.org/developers/cvs.html)
	* [OpenBSD - Security focused, hard code](https://github.com/openbsd/src)
	* [NetBSD - Portable, clean code](https://netbsd.org/docs/guide/en/part-compile.html)

* Educational OSs
	* [xv6(6k lines)](https://github.com/mit-pdos/xv6-public)
	* [BaseKernel - education all OS with all nesecarry parts(interrupts, video...)](https://github.com/dthain/basekernel)
	* [Nachos OS](https://www.cs.washington.edu/homes/tom/nachos/)

* Hobby OSs
	* [Raspberry Pi OS](https://github.com/s-matyukevich/raspberry-pi-os)
	* [Sortix - POSIX Hobby OS](https://gitlab.com/sortix/sortix)
		* Easy C Library implementation - sortix/libc
	* [ToaruOs - GUI, Dynamic libs, network...](https://github.com/klange/toaruos)
	* [OneOS - Unix Like ARM, x86 OS with stack of all OS features, GUI and mobile support](https://github.com/oneOS-Project/oneOS)
	* [TILCK - Small Linux compatible OS](https://github.com/vvaltchev/tilck)

* Releated to OSs
	* [TomatBoot - Modern multi-protocol x86_64 UEFI Bootloader](https://github.com/TomatOrg/TomatBoot)
	* [Limine Bootloader - Stivale x86_64 UEFI/BIOS Bootloader](https://github.com/limine-bootloader/limine)
	* [Simple printf implementation](https://github.com/mpredfearn/simple-printf)



## Implementations
* [Framebuffer](https://www.kernel.org/doc/html/latest/fb/framebuffer.html)
* [ioctl](https://forum.osdev.org/viewtopic.php?f=1&t=41907)
* [PS/2 Keyboard](https://github.com/torvalds/linux/blob/master/drivers/input/keyboard/atkbd.c)
