/*
    @author = eltertrias
    @brief = local defines for pmm
*/


#include <kernel/init.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kstdlib/iolibk.h>
#include <stdint.h>
#include <kstdlib/string.h>


#define FRAME_TYPE_FREE      			0x0			// Can be given on allocation request
#define FRAME_TYPE_ALLOCATED 			0xFF		// Can not be given on allocation request

#define TRUE							1
#define FALSE							0

#define FIND_END(start, size) (start + size - 1)


/*
    @brief = fills different status variables with info about RAM based on the memory map
    @return = total amount of RAM, 0 on error
*/
static int getRAMinfo();

/*
	@brief = initialises the algorithm used to track frames
	@return = 0 on success, -1 on error
*/
static int initFrameTracker();

/*
	@brief = get's the status of a page by it's number
	@param pageNumber = the umber of the page(address / 4096) that status should be got off
	@return = returns the status of the page(0 if FREE and 1 if LOCKED)
*/
static uint8_t getFrameStatus(int pageNumber);

/*
	@brief = locks(marks as used) specified amount of frames starting at index of the specified address
	@param frame_address = address of frame from which reserving should start
	@param frame_count = amount of frames to reserve
	@return = 0 on success , -1 on fail
*/
static int lockRAM(void* frame_address, size_t frame_count);

/*
	@brief = unreserved(marks as free) specified amount of frames starting at index of the specified address
	@param frame_address = address of frame from which unreserving should start
	@param frame_count = amount of frames to unreserve
	@return = 0 on success , -1 on fail
*/
static int unlockRAM(void* frame_address, size_t frame_count);