/*
	@author = eltertrias
	@brief = local header for vmm
*/


#ifndef VMM_DEFINES_H
#define VMM_DEFINES_H

#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kcalls/mm.h>
#include <kernel/init.h>
#include <kstdlib/string.h>
#include <stdint.h>
#include <stddef.h>
#include <kstdlib/iolibk.h>
#include <kstdlib/libk_debug.h>
#include <stdbool.h>
#include "vmm-config.h"


struct vmem_info
{
	uint8_t no_halfs;

	address_tt lower_half_start;
	address_tt lower_half_end;

	address_tt higher_half_start;
	address_tt higher_half_end;

	address_tt kernel_free_space_start_vaddr;	// Grows downwards
	address_tt kernel_free_space_end_vaddr;
	address_tt user_free_space_start_vaddr;	// Grows upwards
	address_tt user_free_space_end_vaddr;

	address_tt identity_mapped_ram_start;
};


/*
    @brief = changes the chosen bit in supplied variable to chosen value(ON(1) or OFF(0))
    @param var = pointer to variable where the changes will be made
    @param bitmask = the bit which has to be changed
    @param bit_status = to which status the bit should be changed
*/
static void toggleBit(size_t* var, size_t bitmask, uint8_t bit_status);

/*
	@brief : determines the size of lower and higher halfs and the number of bit's available
	@return : 0 on success, -1 on fail
*/
static int interpretAddressSpace(void);

/*
	@brief : gets the offsets in all of the page tables
	@param vaddr :: the address from which offsets should be got
	@param offsets : size_t array of "<paging table levels amount> + 1(for the page offset)" size(e.g. 4 on PML4), the offsets in each of the tables would be put into this array. Usage - array[0] - page offset, array[1] - PT offset...
	@return : amount of entries in the array, 0 on error
*/
static size_t getTableOffsets(address_tt vaddr, size_t* offsets);

/*
	@brief : edits the page tables before loading them, to have "size" bytes starting at "paddr" mapped
	@param paddr : the paddr of data to be mapped(should be page-aligned)
	@param vaddr : if supplied with a not page-aligned address the function will choose mapping address by itself, otherwise address in vaddr is used as the mapping address
	@param size : size in bytes of the data to be mapped
	@return : virutal address where the data would be mapped when the page tables are loaded
*/
static void* preMMAP(void* paddr, void* vaddr, size_t size);

/*
	@brief : creates the initial page tables that map kernel to the last 2GB
	@return : 0 on success, -1 on fail
*/
static int createPageTables();

/*
	@brief : maps a page table(or just shows the location where it is mapped) at physical address supplied and returns virtual
	@param paddr : physical address of the page table
	@return : virtual address of page table on success, NULL on fail
*/
static void* mapPageTable(void* paddr);

/*
	@brief : is a common allocation function for both kvalloc and valloc
	@param amount : amount of pages to be mapped
	@param allocation_start_addr : virtual address at which the allocation should start
	@return : NULL on success, 0x1(not 4096 aligned address which impossible in any other case) on fail
*/
static void* local_valloc(size_t amount, address_tt allocation_start_addr);
#endif