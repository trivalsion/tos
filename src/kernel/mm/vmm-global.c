/*
	@author : eltertrias
	@brief : vmm file that has virtual memory related functions exposed to the other kernel parts(kcalls)
*/


#include "include/vmm.h"

#if CPU_BITNESS == 64
#include "vmm64.c"
#else
#include "vmm32.c"
#endif


void* valloc(size_t amount)
{
	if (amount == 0 && vmem_info.user_free_space_start_vaddr >= vmem_info.user_free_space_end_vaddr)
		return MEMORY_MAPPING_ERROR;

	address_tt allocation_start_addr = vmem_info.user_free_space_start_vaddr;
	vmem_info.user_free_space_start_vaddr = allocation_start_addr + (PAGE_SIZE * amount);

	if (local_valloc(amount, allocation_start_addr) == MEMORY_MAPPING_ERROR)
		return MEMORY_MAPPING_ERROR;

	return (void*)allocation_start_addr;
}

void* kvalloc(size_t amount)
{
	if (amount == 0 && vmem_info.kernel_free_space_start_vaddr >= vmem_info.kernel_free_space_end_vaddr)
		return MEMORY_MAPPING_ERROR;

	address_tt allocation_start_addr = vmem_info.kernel_free_space_start_vaddr - (PAGE_SIZE * amount);
	vmem_info.kernel_free_space_start_vaddr = allocation_start_addr;

	if (local_valloc(amount, allocation_start_addr) == MEMORY_MAPPING_ERROR)
		return MEMORY_MAPPING_ERROR;

	return (void*)allocation_start_addr;
}

static void* local_valloc(size_t amount, address_tt allocation_start_addr)
{
	void* current_paddr;

	/* In a loop just obtains random physical addresses from palloc and maps them to the initial_vaddr + amount of iterations it went through */
	for (size_t i = 0; i < amount; i++)
	{
		if ((current_paddr = palloc(1)) == NULL)
			return MEMORY_MAPPING_ERROR;
		if (kmmap(current_paddr, (void*)(allocation_start_addr + (PAGE_SIZE * i)), 1) == MEMORY_MAPPING_ERROR)
			return MEMORY_MAPPING_ERROR;
	}

	return NULL;
}

int vfree(void* address, size_t amount)
{
	if (kunmap(address, amount) == -1)
		return -1;

	return 0;
}