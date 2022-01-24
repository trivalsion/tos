# [Paging](https://wiki.osdev.org/Paging)
- Description - splitting memory into chunck(pages, normally 4096 byte sized) and mapping these chuncks to physical memory by filling in a tree of tables
- Notes:
	* In all 4096 aligned address when converted to binary, first 12 bits are 0
	* All the page tables should be cleared(all bits set to 0) before they are used
- ***WARNING(VERY IMPORTANT):*** **RELOAD CR3(LOAD ADDRESS OF L4(PML4 or PML5) TABLE INTO IT, EVEN IF IT IS ALREADY THERE) AFTER THE PAGE TABLES WERE CHANGED/EDITED.** This is done because when you edit page tables, the cache still contains old entries and do reload it you should reload CR3




# [Page Frame Allocatior/Physical Memory Manager(PFA/PMM) - the PMMs task is to keep record of which physical memory is free and which is not, it does that by seperating memory in page-sized chunks(frames) and keeping track of free/used statuses of these chunks. The way that it marks some memory used, is when it is asked to give out a free memory frame and it gives it out and marks it as used](https://wiki.osdev.org/Page_Frame_Allocation)
## Allocation Algorithm - Bitmap Allocation
+ Description - There exists a bitmap which is a list of bytes and each bit in every byte corresponds to a status of the frame at address `bit_number * 4096`, if the bit is 0 then frame is free and can be allocated, else if it is 1 it is already allocated
+ Algorithm
	* Brief description - a portion of memory is reserved for the "bitmap", bitmap consists of multiple bytes where each bit represents a status of a physical frame(bit off - free, bit on - used). The page that correspnds to the bit and the other way round is found using formulas. So when a frame is allocated the corresponding bit is set and the address of the page is returned to allocation function caller and when the page is freed the bit is turned off.
	* Initialization
		1. Range of memory/size - `RAMsize = highest_mmap_entry_end(last usable byte) - lowest_mmap_entry_start(first usable byte)`
		2. Find largest free memory area in the memory map and place the bitmap(bitmap pointer should be `uint8_t`) at the start of it if there is enough space for it in that memory area - `bitmapSize = RAMsize / 4096 / 8`
		3. Set all of the frame statuses to allocated as not all areas are described in the memory map and so they cannot be assumed as free - `memset(bitmap, 0xFF, bitmapSize)`
		4. Unreserve the free areas in the memory map in the bitmap(unreserve one page less then the size of free memory area)
	* Allocation
		1. Loop through the bitmap and find the first free frame(where bit is 0)
		2. Mark the entry as reserved, entry formula - `bitmap[byteIndex] |= 1 << bitIndex`
		3. Calculate the start address from entry number and return it(it must be 4096 aligned)
		* If multiple frames are required - just find a place where there is the right amount of free frames one after another, reserve them all and return address of the first one
	* Freeing
		1. Calculate bit in bitmap corresponding to the address supplied(must be start address of the frame and 4096 aligned)
		2. Check if the entry number is not beyond the bitmap
		3. Mark the bit(entry) as free, entry formula - `bitmap[byteIndex] &= ~(1 << bitIndex)`
		* If multiple frames are required - just free them one after another starting from the supplied address
	* Formulas
		* Address -> Page Number = `addr / 4096`
		* `byte index = page_number / 8`(byte where the status bit of page is)
		* `bitIndex = frameIndex % 8`(bit which has the status of the page)



## Tests
1. Allocate frames until the palloc returns error and with each frame check:
	* If it is page size(4096) aligned
	* Check that it is located inside a free zone
	* Check if the right or nearly right amount of frames is allocated(in releation to the size of RAM)
	* Record the end address of each previous frame and check if they intersect(new one starts inside the previous), to check if they intersect check if end of previous page is below the start of current, If YES they don't intersect, If NO check if the end of current page is below start of previous - if YES then they don't intersect, otherwise they INTERSECT
		*	```C
			if (previous_page_end < (address_tt)page_frame)
				; // NO COLLISION
			else if (previous_page_start > ((address_tt)page_frame + 4096 - 1))
				; // NO COLLISION
			else
			{
				printf("Page started inside previous page: Current page - %lx, New page - %lx\n", previous_page_end, (address_tt)page_frame);
				break;
			}
			```
	* Write to each frame
2. Sum of all pages that are locked/reserved should be bigger or the same as there is reserved RAM
3. Sum of all pages that are free should be smaller or same as there is free RAM




# [Virtual Memory Manager - manages the page tables by editing and creating them to map physical memory to virtual and return the virtual page to user/kernel](https://wiki.osdev.org/Memory_Allocation)
## Main Algorithm
+ Description - three VMM files should exist, one would have global functions like allocation that could be used by any part of the kernel. Other two files would have initialization, mapping and page table editing functions, but one would have them for the 32bit CPUs and the second one would have them for 64bit CPUs. One for 64bit CPUs would use Identity mapping algorithm to edit page tables and map physical memory, second one for 32bit CPUs would use recursive page tables for editing page tables and a harder algorithm(combining editing multiple page tables) to map physical memory to virtual. Functions in the global file like the allocation function would just recieve an easy interface API that would stay the same between 32bit and 64bit CPUs, for allocation linear allocation/mapping algorithm would be used



## Page Table Editing Algorithm
### 64bit CPUs - Itentity mapping
+ Description - bootloader identity maps the whole memory space so we(OS it loads) could do anything. We create new page tables(allocate physical space for them and edit) where we are mapping the kernel at "last_addr - 2GiB" addr and also where we map all of the RAM(to calculate RAM - add up sizes of all areas in the memory map) at the start of the higher half of memory sapce and also mark the "start of higher half -> (start of higher half + RAM Size)" as reserved so this virtual memory can't be used by allocation functions. To access any physical address including page tables, just add the start of higher half(or other address where identity mapping starts) to it and it would be turned into corresponding virtual address that could be accessed easily.
+ Algorithm
	1. The address space is interpreted and the start and end of lower and higher halves is found
	2. Page tables are created and space for them is allocated with `palloc()`. The page tables are edited so the kernel is mapped into the `end_of_ram - 2GB` address(e.g. 0xffffffff80000000 is 0xFFFFFFFFFFFFFFFF(max 64 bit address) - 0x80000000(2 GiB)). If the bootloader wants, the kernel could be remapped to start at a higher address, but not lower then `end_of_ram - 2GB`.
	3. Loop through page tables and map all of the RAM(to calculate RAM add up sizes of all areas in the memory map) starting at virtual address which is the start of higher half of virtual memory and mark this area as unusable for virtual memory allocation so it is not overwritten
	4. Pages tables are reloaded by changing address in CR3 or similar



## Page Mapping Algorithm
### 64bit CPUs - Giving out the corresponding virtual address from the identity mapped area, as nothing should be mapped again as everything was identity mapped in the start
+ Description - as all of the physical RAM is mapped into virtual, just return the `supplied_paddr + start_of_identity_mapping` and it would give the virtual addr where the asked physical address is mapped
+ Algorithm
	1. Return to the user - `supplied_paddr + start_of_identity_mapping`



## Memory Allocation Algorithm - linear allocation
+ Description - The memory is allocated in a linear way and seperated into two halves(kernel half and user half) the user half grows from 0x0 to the end of the lower half and the kernel half grows from -2 to the start of higher half(the address to which it grows should be changed by adding the amount of memory that was identity mapped so it is not accidentaly allocated and overwritten).



## Notes
* kalloc and alloc shouldn't allocate pas the start/end of their halves of memory
* Data structures that could be used to improve VMM: Black and Red balanced binary trees
* Make sure that when using addresses that are located in page table entries, the flag bits are ignored because CPU does not use them as part of address so the kernel also shoudn't. Flag bits are first 12 bits, so they should be removed(for example using AND)
* Memory Layout
	* 0x0 -> end_of_lower_half = free space
	* higher_half_start -> (higher_half_start + RAM_size) = identity mapping of ram
	* (higher_half_start - (higher_half_start + RAM_SIZE)) -> (highest_addr - 2GB) = free space for allocations for kernel needs(allocation should go downwards starting with (highest_addr - 2GB))
	* (highest_addr - 2GB) -> highest_addr - = space where the kernel is mapped




# Questions
## Solved
- Paging
	+ What if the some data happens to be located between two pages(e.g. several byts on one and others on another) and how to prevent it?
		* This is not a problem ans probably is not even possible
	+ Should the page frame allocator align every page by 4096?
		* Yes because becuase all addressess used for paging structs and pages should be 4096, otherwise first 12 bits would not be zeroes(be free for use)
	+ Where should be the page tables/structs be stored in memory(should some space be allocated for them using PFA)?
- Higher Half Kernel
    + Why is 0xC0000000 used - which is below 4GB which means that even in 64 bit mode programm can only access stuff below 0xc0000000 or how does it work, is it remapped beyond end of kernel as soon as programm alocates all the space below?
        * The 0xC0000000 is used only in 32bit mode because it leaves 3GB free for programms and a little space at the end of memory for the kernel
    + Is there any use in having the kernel physically there(which is even impossible in some cases like if computer has 2GB RAM)?
        * No, it is even impossible to have the kernel located there in most configurations and in 64 bit mode it is beyond what motherboards support
    + What if in 32 bit mode kernel would need more RAM
        * Kernel could just be remapped to a lower address then 0xc0000000 or any other that is used by the kernel




# Resources
- AMD Manual - VOL 2, Chapters 3 & 4
- Intel Manual - VOL 3A, Chapters 3 & 4
- [Wiki on how MM works](https://linux-mm.org/)
- Tutorials
	+ Best
		* [Make Page frame alloctor](https://wiki.osdev.org/Writing_A_Page_Frame_Allocator)
		* [Setup paging](https://wiki.osdev.org/Setting_Up_Paging)
		* [Paging][https://wiki.osdev.org/Paging]
	+ General Memory Management
		* [Bona Fide OSdev](http://www.osdever.net/tutorials/index#Memory-Management)
		* [Brendan's MM Guide](https://wiki.osdev.org/Brendan%27s_Memory_Management_Guide)
    	* [Writing MM](https://wiki.osdev.org/Writing_a_memory_manager)
	+ Physical Memory Management
		* [Bitmap PMM algorithm insturctions](http://shell-storm.org/blog/Physical-page-frame-allocation-with-bitmap-algorithms/)
		* [Bitmap PMM algorithm insturctions 2](https://medium.com/lucians-blog/writing-a-physical-memory-manager-866fc1457d6)
		* [MM Guides from Forum](https://wiki.osdev.org/Page_Frame_Allocation#Threads)
		* [Anastasion Page Frame Allocator Tutorial](https://anastas.io/osdev/memory/2016/08/08/page-frame-allocator.html)
		* Poncho PMM
			* [Part 1](https://www.youtube.com/watch?v=fDGi3uSlQIA&list=WL&index=3)
			* [Part 2](https://www.youtube.com/watch?v=v2bn7fjbnb8&list=WL&index=4)
			* [Bug Fixes](https://www.youtube.com/watch?v=6jme-fSXYJM&list=WL&index=5)
	+ Virtual Memory Management
    	* [James Molloy Paging](http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html)
    	* [Bona Fide Paging](http://www.osdever.net/tutorials/view/implementing-basic-paging)
    + Other
		* [Linux driver for cleaning the memory map](https://github.com/torvalds/linux/blob/f296bfd5cd04cbb49b8fc9585adc280ab2b58624/arch/x86/kernel/e820.c)
		* [Port an existing userspace memory allocator to the OS](https://wiki.osdev.org/Memory_Allocation#Porting_an_existing_Memory_Allocator)
		* [Pancakes memory allocator implementation](https://wiki.osdev.org/User:Pancakes/SimpleHeapImplementation)
		* [Small malloc implementation](https://github.com/CCareaga/heap_allocator)
- Other
	* [Higher Half Kernel Guide on i386](https://medium.com/@connorstack/how-does-a-higher-half-kernel-work-107194e46a64)
	* [Memory info in x86_64](https://en.wikipedia.org/wiki/X86-64#Virtual_address_space_details)



## Extra information
- Memory Allocator Types
	* Physical Memory Manager(PMM)/Page Frame Allocator(PFA) - finds free page-sized physical memory areas
	* Virtual Memory Manager(VMM) - maps virutal pages to physical areas that are obtained from PFA
	* Byte granular allocator/User space allocator - allocates amount of bytes requested by user, that it takes out of a page it obtains from VMM
- [Address spaces](https://wiki.osdev.org/Memory_management#Address_Spaces)
    * Physical Address Space - when address is accessed - it is the real address in RAM that is accessed(e.g. if 0xb8000 is read, the info will be read from 0xb8000s byte in RAM)
    * Virtual Address Space - when address is accessed - it is virtual address that is accessed and it will be translated into physical address that could(but not always) be a completely different address form the one accessed(e.g. if 0xb8000 is read, it could really be absoultely any other byte in RAM that is read, like byte 0x98 in RAM instead of 0xb8000 for example)
- [Higher Half Kernel](https://wiki.osdev.org/Higher_Half_Kernel) - it is a way to make the memory space cleaner and let the programms start at 0x0, while kernel is right at the end of memory
    * **Motivation:** Kernel could be located at any physical address in the memory space(where it was loaded) at the start, but when paging is enabled the kernel can be remapped to any address in the address space becuase addresses become virtual. When paging is enabled it could be made that each program has it's own memory space, if the kernel is mapped at the end of memory then the programs recieve all the other memory space for them and can be loaded at 0x0 and use everything till the start of kernel which makes linking the application very easy(no need to take into account that first mb+ is taken up)
    * **Implementation:** The kernel and the space belowow 1mb(on x86) is remapped using paging to be right at the end of memory(so from their start to end of memory there is left only enough space to fit them)
		* There would be variables placed in the linker_script before the kernel and after the kernel to calculate it's size `kernel_size = kernel_end-kernel_start`, then the address where kernel should be reallocated would be counted with - if kernel_size < 512MB then: `kernel_vma = max_address(available in architecture) - 512MB`; if kernel_size > 512MB then - `kernel_vma = max_address(available in arch) - (kernel_size + 512MB)`
- [Identity Paging](https://wiki.osdev.org/Identity_Paging) - when the pages are mapping virtual addresses 1 to 1 whith physical(e.g. 0xb8000 is 0xb8000)
- [Recursive Paging](https://wiki.osdev.org/User:Neon/Recursive_Paging) - a way how page tables could be used without identity mapping
	* [Creating Recursive Paging](https://taptipalit.blogspot.com/2013/10/theory-recursive-mapping-page.html)
	* Alternatives and discussions
		* [Osdev](https://forum.osdev.org/viewtopic.php?f=1&t=36910)
		* [Ycombinator](https://news.ycombinator.com/item?id=19017995)
	* Description - one of the entry in the highest level page table(e.g. PML4) contains not the address of a page table of next level, but points to PML4 casuing an endless loop, and by looping through a certain amount of times we can access any address we need, we just need to have a different offset at the entry in the last page table as we need to make it interpret table as a page, by default accessing `0x<L4_entry><l4_entry><l4_entry><l4_entry>000` causes us to access the l4 page table as a page.
	* Setup
		* Have a certain offset in highest level page table(e.g. pml4) contain address of the same highest level page table(e.g. PML4) - e.g. `L4[0xFF] = L4`
			* On 64bit architrecture it can't be the last entry as kernel is mapped there, it is better to use the entry at the start of the higher half of memory space as it is easier to mark as "unusable virtual memory"

	* ```

		Usage:
			Map L4 page table - So if you mapped the last page offset of highest page table to it's start then it is 0xff. And so to access L4 we just
			access 0xff'ff'ff'ff'ff'000, and that is because CPU interprets it as:
			L3 = L4[FF];
			L2 = L3[FF]; // because of previous step L3[FF] == L4[FF] and  because L4[FF] == L4
			L1 = L2[FF]; // because of previous step L2[FF] == L3[FF] and L3[FF] = L4[FF] because of the step before while L4[FF] == L4
			Page = L1[FF] // and L1[FF] == L4[FF] and l4[FF] equals to the start of L4 and so L4 table is mapped like a page

			And so we could just access page at 0xff'ff'ff'ff'ff'000 and that would be accessing L4 table where we could change address and flags in entry at any offset
		To access chosen L3 page table in the L4 page table we should replace last "ff" with the offset of the needed L3 page table and then it wo
		uld map it as a page
		```