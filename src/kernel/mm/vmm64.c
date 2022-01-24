/*
	@author = eltertrias
	@brief = manages the virtual memory and page tables in 64bit CPUs
*/


#include "include/vmm.h"


//#define VMM_DEBUG


static struct vmem_info vmem_info;
static address_tt* kernel_page_table_l4_paddr;


static void toggleBit(size_t* var, size_t bitmask, uint8_t bit_status)
{
    if ((bit_status == TOGGLE_BIT_ON && (*var & bitmask)) || (bit_status == TOGGLE_BIT_OFF && !(*var & bitmask)))
        ;
    else if ((bit_status == TOGGLE_BIT_ON && ((*var & bitmask) != bitmask)) || (bit_status == TOGGLE_BIT_OFF && ((*var & bitmask) == bitmask)))
        *var ^= bitmask;
}

static int interpretAddressSpace()
{
	vmem_info.lower_half_start = 0x0;
	vmem_info.higher_half_end = UINTPTR_MAX;

	if (USABLE_ADDRESS_BITS != (sizeof(uintmax_t) * 8))	// Check if the memory space is split in halves - if the amount of address bits is not the same as amount of usable bits in a register(register size by 8(amount of bits in a byte)) then there are halves
	{
		vmem_info.lower_half_end = ((uintptr_t)1 << (USABLE_ADDRESS_BITS - 1)) - 1;	// 1 is converted to uintptr_t so it is large in size(64 on 64 bit computer) and then we move the bit 0 which is set in one to position 47 and subtract one making it turn from 10... to 7FFFF...
		vmem_info.higher_half_start = ~vmem_info.lower_half_end; // It is the opposite of lower half end as in lower half end bits 0-46 are set and 47-63 aren't, so we just turn that around using ones completement
		vmem_info.no_halfs = false;
	}
	else
		vmem_info.no_halfs = true;

	vmem_info.kernel_free_space_start_vaddr = KERNEL_FREE_SPACE_VADDR;
	vmem_info.kernel_free_space_end_vaddr = vmem_info.lower_half_end;
	vmem_info.user_free_space_start_vaddr = USER_FREE_SPACE_VADDR;
	vmem_info.user_free_space_end_vaddr = vmem_info.higher_half_start;

	return 0;
}

static size_t getTableOffsets(address_tt vaddr, size_t* offsets)
{
	size_t i;

	offsets[0] = vaddr & ADDRESS_PAGE_OFFSET_BITMASK;	// The first 12 bits are the offset inside the page so they are got seperatly(as offset of page table/page is 9 bits from the address)
	vaddr >>= ADDRESS_PAGE_OFFSET_BITSIZE;	// Shift off the first 12 bits as they are only for the offset inside the page which is already recorded
	for (i = 1; i < (PAGE_TABLE_LEVELS_AMOUNT+1); i++)
	{
		offsets[i] = vaddr & ADDRESS_PAGE_TABLE_OFFSET_BITMASK;	// Get the first 9 bits and put them as current level page table offset
		vaddr >>= ADDRESS_PAGE_TABLE_OFFSET_BITSIZE;	// Shift off first 9 bits to get the next 9 bits corresponding to offset in the next page table level(higher level)
	}

	return i;
}

static void* preMMAP(void* paddr, void* vaddr, size_t size)
{
	/* LIMITATION - CAN NOT MAP MORE THEN 512 GB OR SOMETHING ON BOUNDARIES OF TWO L3(512GB) TABLES */
	if (kernel_page_table_l4_paddr == NULL || ((size_t)paddr % PAGE_SIZE) != 0 )	// Make sure it can't be executed more then 1 time
		return NULL;

	size_t currently_mapped_bytes = 0;
	size_t page_table_offsets[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt* page_tables[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt data_mapping_vaddr = (vmem_info.kernel_free_space_start_vaddr-1) - size;

	if (((size_t)vaddr % PAGE_SIZE) == 0)
		data_mapping_vaddr = (address_tt)vaddr;

	if (getTableOffsets(data_mapping_vaddr, page_table_offsets) == 0)
		return NULL;

	page_tables[3] = (void*)(kernel_page_table_l4_paddr[page_table_offsets[4]] & ~((size_t)ADDRESS_PAGE_OFFSET_BITMASK)); // Discard the flags from the address by ANDing with a constant where flag bits are zeroed
	if (!(kernel_page_table_l4_paddr[page_table_offsets[4]] & (PAGE_FLAG_PRESENT | PAGE_FLAG_RW)))
	{
		if ((kernel_page_table_l4_paddr[page_table_offsets[4]] = (address_tt)palloc(1)) == (address_tt)NULL)	/* Allocate page sized area for level 3 page table, put it's address in l4 page table entry, clean it with zeroes and set bit flags on the l3 address in l4 page table entry */
			return NULL;
		page_tables[3] = (address_tt*)kernel_page_table_l4_paddr[page_table_offsets[4]];
		memset(page_tables[3], 0, PAGE_SIZE);
		toggleBit(&kernel_page_table_l4_paddr[page_table_offsets[4]], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
	}

	/*
		Allocate paged sized areas for amount of level 2 pages as are required to fit the kernel(each one addresses 1 GB), put the addresses of these areas into l3 table and set RW and present flags on all of them. Each time a level 2 page table is allocated, page sized areas are allocated for level 1 page tables, their addresses are put into current l2 table and the R/W and present flags are set, the amount of level 1 page tables that are created is up to the last entry in current level 2 table or just the amount required to fit the kernel. Finally the pages in level 1 page table are mapped to the physical location of the kernel and present and wright flags are set on them
	*/
	for (size_t current_l3_page_table_offset = 0; (page_table_offsets[3] + current_l3_page_table_offset) <= ADDRESS_PAGE_TABLE_OFFSET_BITMASK && currently_mapped_bytes < size; current_l3_page_table_offset++)
	{
		page_tables[2] = (address_tt*)page_tables[3][page_table_offsets[3] + current_l3_page_table_offset];
		if (!(page_tables[3][page_table_offsets[3] + current_l3_page_table_offset] & (PAGE_FLAG_PRESENT | PAGE_FLAG_RW)))
		{
			if ((page_tables[3][page_table_offsets[3] + current_l3_page_table_offset] = (address_tt)palloc(1)) == (uintptr_t)NULL)	/* Create a level 2 page table */
				return NULL;
			page_tables[2] = (address_tt*)page_tables[3][page_table_offsets[3] + current_l3_page_table_offset];
			memset(page_tables[2], 0, PAGE_SIZE);
			toggleBit(&page_tables[3][page_table_offsets[3] + current_l3_page_table_offset], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
		}

		for (size_t current_l2_page_table_offset = 0; (page_table_offsets[2] + current_l2_page_table_offset) <= ADDRESS_PAGE_TABLE_OFFSET_BITMASK && currently_mapped_bytes < size; current_l2_page_table_offset++)
		{
			page_tables[1] = (address_tt*)page_tables[2][page_table_offsets[2] + current_l2_page_table_offset];
			if (!(page_tables[2][page_table_offsets[2] + current_l2_page_table_offset] & (PAGE_FLAG_PRESENT | PAGE_FLAG_RW)))
			{
				if ((page_tables[2][page_table_offsets[2] + current_l2_page_table_offset] = (address_tt)palloc(1)) == (uintptr_t)NULL)	/* Create level 1 page table */
					return NULL;
				page_tables[1] = (address_tt*)page_tables[2][page_table_offsets[2] + current_l2_page_table_offset];
				memset(page_tables[1], 0, PAGE_SIZE);
				toggleBit(&page_tables[2][page_table_offsets[2]], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
			}

			for (size_t current_l1_page_table_offset = 0; (page_table_offsets[1] + current_l1_page_table_offset) <= ADDRESS_PAGE_TABLE_OFFSET_BITMASK && currently_mapped_bytes < size; current_l1_page_table_offset++)
			{
				page_tables[1][page_table_offsets[1] + current_l1_page_table_offset] = (address_tt)paddr + currently_mapped_bytes;	/* Map the pages to physical location of the kernel */
				toggleBit(&page_tables[1][page_table_offsets[1] + current_l1_page_table_offset], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
				currently_mapped_bytes += PAGE_SIZE;
			}
		}
	}

	return (void*)data_mapping_vaddr;
}

static int createIdentityMapping()
{
	if (kernel_page_table_l4_paddr == NULL)	// Check if page tables are created
		return -1;

	size_t real_ram_amount = 0;
	tosBootProtocol_tt* bootInfo = getBootInfo();

	size_t mapped_memory = 0;
	address_tt* page_tables[PAGE_TABLE_LEVELS_AMOUNT+1];
	size_t page_table_offsets[PAGE_TABLE_LEVELS_AMOUNT+1];

	for (size_t i = 0; bootInfo->mmap[i].flags & MEMINFO_FLAG_PRESENT; i++)	// Count the real amount of RAM by adding the sizes of all size areas in the memory map
		real_ram_amount += bootInfo->mmap[i].area_size;

	vmem_info.identity_mapped_ram_start = vmem_info.higher_half_start; // Space where the memory is identity mapped is at the start of the higher half
	vmem_info.kernel_free_space_end_vaddr += real_ram_amount; // Chage the end of kernel free space so the identity mapping isn't touched during allocation
	while ((vmem_info.kernel_free_space_end_vaddr % PAGE_SIZE) != 0)	// Make sure that area where RAM is identity mapped won't be touched by kernel allocator by marking a page more as unmarked
		vmem_info.kernel_free_space_end_vaddr += 1;

	if (getTableOffsets(vmem_info.identity_mapped_ram_start, page_table_offsets) == 0) // Should be after vmem_info.identity_mapped_ram_start variable is set to thecorrect value
		return -1;

	/*
		Start with looping through l4, starting at an entry given by getTableOffsets for the higher half start address. At this start entry create an l3 table by putting a physical address into the entry and setting flags on it(also cleaning it with 0s). Next loop through the entries in the newly created l3 starting with the one given by getTableOffsets, in the first one(starting from one given by getTableOffsets) create an l2 table(allocate, set flags, clean). Inside the l2 table start doing the same thing by creating an l1 table, in level1 table put the current physical address from variable tracking that is tracking mapping progress(increase each time and start with 0) into the entry for the page(mapping it to that physical address). When there are no more page entries in l1, loop switches to the next cycle inside l2 and so create the next l1, when l2 finishes the loop switches to next cycle inside l3 and a new l2 is created and proccess starts again and repeats allocating tables and mapping pages until everything is mapped. If the mapped memory is same or more then real, then everything was identity mapped and the loop terminates.
	*/
	size_t l4_i = page_table_offsets[4];	/* The loop indexes should start with offsets only at the first instance of each loop */
	size_t l3_i = page_table_offsets[3];
	size_t l2_i = page_table_offsets[2];
	size_t l1_i = page_table_offsets[1];
	for (; l4_i < 512 && mapped_memory <= real_ram_amount; l4_i++) /* Loop through l4 and create an l3 in the selected entry, if l2s finish(no more could be created as all entries are filled) inside that l3, create next l3 table in the next entry of l4. All loops after this do the same thing, but they create not a new l3, but for example loop inside l2 creates new l1 enries and inside l1 new pages are mapped. */
	{
		if ((kernel_page_table_l4_paddr[l4_i] = (address_tt)palloc(1)) == (address_tt)NULL)	/* Allocate space for the table, clean it with 0s, set flgas on it's entry in the l4 and put the current l3 address into the arrays entry for current l2. All contents of the loop after this do the same thing, just inside other page tables */
			return -1;
		memset((void*)kernel_page_table_l4_paddr[l4_i], 0, PAGE_SIZE);
		page_tables[3] = (address_tt*)kernel_page_table_l4_paddr[l4_i];
		toggleBit(&kernel_page_table_l4_paddr[l4_i], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);

		for (; l3_i < 512 && mapped_memory <= real_ram_amount; l3_i++)
		{
			if ((page_tables[3][l3_i] = (address_tt)palloc(1)) == (address_tt)NULL)
				return -1;
			memset((void*)page_tables[3][l3_i], 0, PAGE_SIZE);
			page_tables[2] = (address_tt*)page_tables[3][l3_i];
			toggleBit(&page_tables[3][l3_i], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);

			for (; l2_i < 512 && mapped_memory <= real_ram_amount; l2_i++)
			{
				if ((page_tables[2][l2_i] = (address_tt)palloc(1)) == (address_tt)NULL)
					return -1;
				memset((void*)page_tables[2][l2_i], 0, PAGE_SIZE);
				page_tables[1] = (address_tt*)page_tables[2][l2_i];
				toggleBit(&page_tables[2][l2_i], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);

				for (; l1_i < 512 && mapped_memory <= real_ram_amount; l1_i++)
				{
					memset((void*)page_tables[1][l1_i], 0, PAGE_SIZE);
					page_tables[1][l1_i] = mapped_memory;
					toggleBit(&page_tables[1][l1_i], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
					mapped_memory += PAGE_SIZE;
				}
				l1_i = 0; // Reset counter after the loop finishes
			}
			l2_i = 0;
		}
		l3_i = 0;
	}

	return 0;
}

static void* getVirtualAddr(void* paddr)
{
	return (paddr + vmem_info.identity_mapped_ram_start);
}

static int createPageTables()
{
	if (kernel_page_table_l4_paddr != NULL) // Make sure it can't be executed more then 1 time
		return -1;

	tosBootProtocol_tt* bootInfo = getBootInfo();
	struct mem_info* kernel_paddr_info;
	address_tt kernel_default_load_vaddr = vmem_info.higher_half_end - TWO_GB;	// Kernels default vaddr should be end of memory space -2GB
	size_t page_table_offsets[PAGE_TABLE_LEVELS_AMOUNT+1];

	if (bootInfo->vla_info.flags & LOAD_INFO_FLAG_PRESENT)	/* Get the kernel load address from the boot loader(if present) and make sure it is not lower then -2GB address */
	{
		if (bootInfo->vla_info.kernel_load_vaddr > kernel_default_load_vaddr)
			kernel_default_load_vaddr = bootInfo->vla_info.kernel_load_vaddr;
		else
			return -1;
	}

	for (size_t j = 0; bootInfo->mmap[j].flags & MEMINFO_FLAG_PRESENT; j++)	/* Get information about kernel physical location(e.g. size) by parsing memory map supplied by the bootloader */
	{
		if (bootInfo->mmap[j].area_type == MEMMAP_AREA_TYPE_KERNEL)
			kernel_paddr_info = &bootInfo->mmap[j];
	}

	if (getTableOffsets(kernel_default_load_vaddr, page_table_offsets) == 0)
		return -1;

	if ((kernel_page_table_l4_paddr = palloc(1)) == NULL)	/* Allocate page sized physical memory area for level 4 page table and clean it with zeroes */
		return -1;
	memset(kernel_page_table_l4_paddr, 0, PAGE_SIZE);

	if ( (kernel_page_table_l4_paddr[page_table_offsets[4]] = (address_tt)palloc(1)) == (address_tt)NULL)	/* Allocate page sized area for level 3 page table, put it's address in l4 page table entry, clean it with zeroes and set bit flags on the l3 address in l4 page table entry */
		return -1;
	address_tt* kernel_page_table_l3_paddr = (address_tt*)kernel_page_table_l4_paddr[page_table_offsets[4]];
	memset(kernel_page_table_l3_paddr, 0, PAGE_SIZE);
	toggleBit(&kernel_page_table_l4_paddr[page_table_offsets[4]], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);

	if (preMMAP((void*)kernel_paddr_info->start_address, (void*)kernel_default_load_vaddr, kernel_paddr_info->area_size) == NULL)	/* Edited the page tables to have kernel mapped there before they are loaded*/
		return -1;

	if (createIdentityMapping() == -1)	// Idnetity map the memory
		return -1;

	return 0;
}

static void* mapPageTable(void* paddr)
{
	if (((address_tt)paddr % PAGE_SIZE) != 0)
		return NULL;

	return (paddr + vmem_info.identity_mapped_ram_start);
}

void* kmmap(void* paddr, void* vaddr, size_t amount)
{
	if (((address_tt)vaddr % PAGE_SIZE) != 0 || ((address_tt)paddr % PAGE_SIZE) != 0 || kernel_page_table_l4_paddr == NULL || amount == 0)
		return MEMORY_MAPPING_ERROR;

	size_t table_offsets[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt* page_tables[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt current_paddr = (address_tt)paddr;
	address_tt current_vaddr = (address_tt)vaddr;

	for (size_t i = 0; i < amount; i++)
	{
		if (getTableOffsets((address_tt)current_vaddr, table_offsets) == 0)
			return MEMORY_MAPPING_ERROR;
		if ((page_tables[PAGE_TABLE_LEVELS_AMOUNT] = mapPageTable(kernel_page_table_l4_paddr)) == NULL)
				return MEMORY_MAPPING_ERROR;

		/*
			It loops through all of the page table levels, obtaining the address of the next one by going to the right offset in the current one or creating a new page table if it does not exist, it does that until it reaches level 1 page table where it just fills the page offset with the physical address. And it does all of that in a loop for any amount of pages.
		*/
		for (size_t current_page_table_level = PAGE_TABLE_LEVELS_AMOUNT; current_page_table_level > 0; current_page_table_level--)
		{
			if (!(page_tables[current_page_table_level][table_offsets[current_page_table_level]] & (PAGE_FLAG_RW | PAGE_FLAG_PRESENT)))
			{
				if ((page_tables[current_page_table_level][table_offsets[current_page_table_level]] = (address_tt)palloc(1)) == (address_tt)NULL)
					return MEMORY_MAPPING_ERROR;
				memset(getVirtualAddr((void*)page_tables[current_page_table_level][table_offsets[current_page_table_level]]), 0, PAGE_SIZE);
				toggleBit(&page_tables[current_page_table_level][table_offsets[current_page_table_level]], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);
			}
			if ((page_tables[current_page_table_level-1] = mapPageTable((void*)(page_tables[current_page_table_level][table_offsets[current_page_table_level]] & ~ADDRESS_PAGE_OFFSET_BITMASK))) == NULL)
				return MEMORY_MAPPING_ERROR;
		}

		page_tables[1][table_offsets[1]] = (address_tt)current_paddr;
		toggleBit(&page_tables[1][table_offsets[1]], PAGE_FLAG_PRESENT | PAGE_FLAG_RW, TOGGLE_BIT_ON);

		current_vaddr += PAGE_SIZE;
		current_paddr += PAGE_SIZE;
	}

	arch_loadPageTables(kernel_page_table_l4_paddr);

	return vaddr;
}

int kunmap(void* vaddr, size_t amount)
{
	if (((address_tt)vaddr % PAGE_SIZE) != 0 || kernel_page_table_l4_paddr == NULL || amount == 0)
		return -1;

	size_t table_offsets[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt* page_tables[PAGE_TABLE_LEVELS_AMOUNT+1];
	address_tt current_vaddr = (address_tt)vaddr;

	for (size_t i = 0; i < amount; i++)
	{
		if (getTableOffsets((address_tt)current_vaddr, table_offsets) == 0)
			return -1;
		if ((page_tables[PAGE_TABLE_LEVELS_AMOUNT] = mapPageTable(kernel_page_table_l4_paddr)) == NULL)
				return -1;

		// Same algorithms as in kmmap, but it does not create new page tables and the entry for the page in level 1 table is not filled, but zeroe
		for (size_t current_page_table_level = PAGE_TABLE_LEVELS_AMOUNT; current_page_table_level > 0; current_page_table_level--)
		{
			if (!(page_tables[current_page_table_level][table_offsets[current_page_table_level]] & (PAGE_FLAG_RW | PAGE_FLAG_PRESENT)))
				return -1;
			if ((page_tables[current_page_table_level-1] = mapPageTable((void*)(page_tables[current_page_table_level][table_offsets[current_page_table_level]] & ~ADDRESS_PAGE_OFFSET_BITMASK))) == NULL)
				return -1;
		}

		page_tables[1][table_offsets[1]] = (address_tt)NULL;
		current_vaddr += PAGE_SIZE;
	}

	arch_loadPageTables(kernel_page_table_l4_paddr);

	return 0;
}

static void debugVMM(void)
{
	#ifdef VMM_DEBUG
	/* Test page table mapping function */
	uint8_t* table_mapping_test_mapping_area;
	if ((table_mapping_test_mapping_area = mapPageTable((void*)0x808000)) == NULL)	// Map the frame buffer instead of the page table, just to test mapping capabilities
	{
		debugPrintf("Failed to map the data using mapPageTable()");
		while(1)
			;
	}
	*table_mapping_test_mapping_area = 0xDF;
	if (*table_mapping_test_mapping_area != 0xDF)
	{
		debugPrintf("mapPageTable(): Result - FAILURE, Data(should be 0xDF) - %lx\n", *table_mapping_test_mapping_area);
		while(1)
			;
	}
	else
		debugPrintf("mapPageTable(): Result - SUCCESS, Data(should be 0xDF) - %lx\n", *table_mapping_test_mapping_area);

	/* Test data mapping function */
	uint8_t* kmmap_phys_frame;
	uint8_t* kmmap_mapping_area;
	size_t mapping_area_page_size = 5;
	if ((kmmap_phys_frame = palloc(1)) == NULL)	// Allocate a physical frame
	{
		debugPrintf("Failed to allocate physical memory");
		while(1)
			;
	}
	if ((kmmap_mapping_area = kmmap(kmmap_phys_frame, (void*)vmem_info.user_free_space_start_vaddr, mapping_area_page_size)) == MEMORY_MAPPING_ERROR)	// Map several physical frames to a free virtual address
	{
		debugPrintf("Failed to map physical memory");
		while(1)
			;
	}
	*(kmmap_mapping_area + (PAGE_SIZE*(mapping_area_page_size-1))) = 0xBA;	// Write to the last mapped frame which was mapped(several were)
	if (*(kmmap_mapping_area + (PAGE_SIZE*(mapping_area_page_size-1))) != 0xBA)	// Check if the write was successfull(then all frames were mapped successfully)
	{
		debugPrintf("kmmap(): Result - FAILURE, Data(should be 0xBA) - %lx\n", *(kmmap_mapping_area + (PAGE_SIZE*4)));
		while(1)
			;
	}
	else
		debugPrintf("kmmap(): Result - SUCCESS, Data(should be 0xBA) - %lx\n", *(kmmap_mapping_area + (PAGE_SIZE*4)));

	/* Test unmapping function */
	debugPrintf("kunmap() test, make sure %lu consecutive virtual pages starting at address %lx are not mapped to physical memory\n", mapping_area_page_size, kmmap_mapping_area);
	if (kunmap(kmmap_mapping_area, mapping_area_page_size) == -1)
	{
		debugPrintf("Failed to unmap virtual memory");
		while (1)
			;
	}

	/* Test allocation functions */
	uint8_t* valloc_allocated_area;
	uint8_t* kvalloc_allocated_area;
	size_t allocated_area_size = 7;

	if ((valloc_allocated_area = valloc(allocated_area_size)) == MEMORY_MAPPING_ERROR)
	{
		debugPrintf("valloc() returned failure");
		while(1)
			;
	}
	if ((kvalloc_allocated_area = kvalloc(allocated_area_size)) == MEMORY_MAPPING_ERROR)
	{
		debugPrintf("kvalloc() returned failure");
		while(1)
			;
	}

	*(valloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))) = 0xAA;	// Write to the last mapped frame which was mapped(several were)
	if (*(valloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))) != 0xAA)	// Check if the write was successfull(then all frames were mapped successfully)
	{
		debugPrintf("valloc(): Result - FAILURE, Data(should be 0xAA) - %lx\n", *(valloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))));
		while(1)
			;
	}
	else
		debugPrintf("valloc(): Result - SUCCESS, Data(should be 0xAA) - %lx\n", *(valloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))));

	*(kvalloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))) = 0xFF;	// Write to the last mapped frame which was mapped(several were)
	if (*(kvalloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))) != 0xFF)	// Check if the write was successfull(then all frames were mapped successfully)
	{
		debugPrintf("kvalloc(): Result - FAILURE, Data(should be 0xFF) - %lx\n", *(kvalloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))));
		while(1)
			;
	}
	else
		debugPrintf("kvalloc(): Result - SUCCESS, Data(should be 0xFF) - %lx\n", *(kvalloc_allocated_area + (PAGE_SIZE*(allocated_area_size-1))));

	/* Test freeing function */
	debugPrintf("vfree() test: make sure %lu virtual pages starting at %lx and %lu pages starting at %lx are not mapped to physical memory\n", allocated_area_size, valloc_allocated_area, allocated_area_size, kvalloc_allocated_area);
	if (vfree(valloc_allocated_area, allocated_area_size) == -1)
	{
		debugPrintf("vfree(): Failed to free pages mapped with valloc");
		while (1)
			;
	}

	if (vfree(kvalloc_allocated_area, allocated_area_size) == -1)
	{
		debugPrintf("vfree(): Failed to free pages mapped with kvalloc");
		while (1)
			;
	}
	#endif
}

int initVMM(void** pmm_data_addr, size_t pmm_data_size)
{
	if (interpretAddressSpace() != 0)
		return -1;

	if (createPageTables() != 0)
		return -1;

	*pmm_data_addr = getVirtualAddr(*pmm_data_addr);
	REMOVE_UNUSED_WARNING(pmm_data_size);

	if (arch_platformPagingInit(kernel_page_table_l4_paddr) != USABLE_ADDRESS_BITS)
		return -1;

	arch_loadPageTables(kernel_page_table_l4_paddr);

	debugVMM();

	while(1)		// Further instructions may try to use the framebuffer which is not mapped yet and so should not be used
		;

	return 0;
}