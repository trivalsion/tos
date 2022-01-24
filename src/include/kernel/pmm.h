/*
    @author = eltertrias
    @brief = global defines for pmm
*/


#ifndef PMM_H
#define PMM_H


#include <stdint.h>
#include <stddef.h>


/*
	@breif = initialises the PMM(physical memory manager)
	@return = 0 on success, -1 on fail
*/
extern int initPMM(void*** bitmap_addr, size_t* bitmap_size);

/*
	@brief = allocates specified amount of continues frames and returns there start address
	@param frame_count = amount of continous frames that should be allocated
	@return = address of the first frame out of the allocated ones, NULL on error
*/
void* palloc(size_t frame_count);

/*
	@brief = frees specified amount of frames starting at index of the specified start address of a frame
	@param frame_address = from which frame to start freeing
	@param frame_count = amount of frames to free
	@return = 0 on success, -1 on fail
*/
int pfree(void* frame_address, size_t frame_count);
#endif