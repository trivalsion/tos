/*
	@author = eltertrias
	@brief = global header for vmm
*/


#include <stdint.h>
#include <stddef.h>
#include <kstdlib/kernel_types.h>
#include "kcalls/mm.h"


#define PAGE_SIZE 4096

#define TOGGLE_BIT_ON   1
#define TOGGLE_BIT_OFF  0

#define PAGE_FLAG_PRESENT 	0x1
#define PAGE_FLAG_RW		0x2
#define PAGE_FLAG_ACCESS	0x4


/*
	@breif = turns the paging on
	@return = -1 on fail, 0 on success
*/
extern int initVMM(void** pmm_data_addr, size_t pmm_data_size);

/*
	@brief = turn everything on that is needed for paging, but do not start it(as it is work of loadPageTables)
	@return = 0 on fail, number of used address bits in the architecture on success(not the last usable bit(e.g. 47 onn PML4, but the amount of bits - 48 on PML4))
*/
extern uint32_t arch_platformPagingInit(void* page_table);

/*
	@brief = reads the paging register with the address of highest level page table
	@return = returns in the special register
*/
extern address_tt arch_readPageTables(void);

/*
	@brief = loads the paging register with address address of highest level page table
	@param address = the address that is loaded
*/
extern void arch_loadPageTables(void* address);

/*
	@brief : returns the physical address of the stack(from the stack register)
	@return : physical address of the stack
*/
extern address_tt arch_getStackAddress(void);