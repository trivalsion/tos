/*
	@author : eltertrias
	@brief : includes the prototypes of memory management functions accessible to the whole kernel and not only memory manager
*/


#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>
#include <kstdlib/kernel_types.h>


#define MEMORY_MAPPING_ERROR 		(void*)0x1


/*
	@brief : maps specified amount of physical memory frames at a specified virtual memory address
	@param paddr : the physical start address of the page(s) that should be mapped into virtual space(should be page aligned)
	@param vaddr : the virtual start address of the memory where physical frames would be mapped to(should be page aligned)
	@return : 0x1(not 4096 aligned address which impossible to collide with correct address as they are all page aligned) on fail, virtual start address of mapping on success
*/
extern void* kmmap(void* paddr, void* vaddr, size_t amount);

/*
	@brief : unmaps specified amount of pages RAM starting from a specified virtual address by zeroing entries for these pages in level 1 page tables
	@param vaddr : the start address of area that should be unmapped
	@param amount : the amount of pages that should be unmapped
	@return : 0 on success, -1 on fail
*/
extern int kunmap(void* vaddr, size_t amount);

/*
	@brief : allocates <amount> pages one after another and returns address of the first one to the caller. The pages are mapped in the user/lower half of virtual memory
	@param amount : amount of pages to allocate
	@return : 0x1(not 4096 aligned address which impossible in any other case) on fail, virtual address of the first mapped page on success
*/
extern void* valloc(size_t amount);

/*
	@brief : allocates <amount> pages one after another and returns address of the first one to the caller. The pages are mapped in the kernel/higher half of virtual memory
	@param amount : amount of pages to allocate
	@return : 0x1(not 4096 aligned address which impossible in any other case) on fail, virtual address of the first mapped page on success
*/
extern void* kvalloc(size_t amount);

/*
	@brief : frees pages allocated by valloc or kvalloc
	@param address : address of the first page to be freed
	@param amount : amount of pages to be freed
	@return : 0 on success, -1 on fail
*/
extern int vfree(void* address, size_t amount);
#endif