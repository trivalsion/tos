/*
    @author = eltertrias
    @breif = physicaly memory manager(page frame allocator)
	@NOTE: End = start + size - 1(end is last usable byte)
*/


#include "include/pmm.h"
#include <kstdlib/libk_debug.h>


//#define PMM_DEBUG


static size_t usedRAM, freeRAM;
static address_tt RAMstart, RAMsize, RAMend;
static uint8_t* bitmap;
static size_t bitmap_bytesize;
static address_tt kernel_addr;
static size_t kernel_size;


static int getRAMinfo()
{
	static uint8_t RAMinfoStatus = FALSE;
	if (RAMinfoStatus == TRUE)
		return -1;

	uint8_t kernel_area_presence = FALSE;
	tosBootProtocol_tt* bootInfo = getBootInfo();

	for (size_t i = 0; bootInfo->mmap[i].flags & MEMINFO_FLAG_PRESENT; i++)
	{
		if (i == 0)											// Set the end and start to the first area in memmap in case there is no more
		{
			RAMstart = bootInfo->mmap[i].start_address;
			RAMend = bootInfo->mmap[i].start_address + bootInfo->mmap[i].area_size - 1;
		}

		if (bootInfo->mmap[i].area_type == MEMMAP_AREA_TYPE_KERNEL)	/* Get the location and size of the kernel */
		{
			kernel_area_presence = TRUE;
			kernel_addr = bootInfo->mmap[i].start_address;
			kernel_size = bootInfo->mmap[i].area_size;
		}

		if (bootInfo->mmap[i].start_address < RAMstart)			// Change the RAM start if something lower then first entry is found
			RAMstart = bootInfo->mmap[i].start_address;

		if (bootInfo->mmap[i].start_address > RAMend)				// Change the RAM end if something higher then first entry is found
			RAMend = bootInfo->mmap[i].start_address + bootInfo->mmap[i].area_size - 1;

		RAMsize += bootInfo->mmap[i].area_size;

		debugPrintf("addr = 0x%lx, length = 0x%lx, type = 0x%x\n", bootInfo->mmap[i].start_address, bootInfo->mmap[i].area_size, bootInfo->mmap[i].area_type);
	}
	if (kernel_area_presence == FALSE)	/* Fail if kernel area is not specified in the memory map */
		return -1;

	RAMinfoStatus = TRUE;

	return 0;
}

static int initFrameTracker()
{
	size_t largest_mmap_area_size = 0;
	tosBootProtocol_tt* bootInfo = getBootInfo();

	bitmap_bytesize = RAMsize / PAGE_SIZE / 8;

	for (size_t i = 0; bootInfo->mmap[i].flags & MEMINFO_FLAG_PRESENT; i++)
	{
		if (bootInfo->mmap[i].area_type == MEMMAP_AREA_TYPE_USABLE && bootInfo->mmap[i].area_size > largest_mmap_area_size)	/* Finding the largest free memory area and putting it's address and size into bitmap variables as it will be stored right at the start of it */
		{
			bitmap = (uint8_t*)bootInfo->mmap[i].start_address;
			largest_mmap_area_size = bootInfo->mmap[i].area_size;
		}
	}

	// IF PROBLEMS OCCUR - MAKE SURE THAT THE KERNEL AREA IS NOT MARKED AS FREE IN THE MEMORY MAP OR OTHERWISE BITMAP COULD BE WRITTEN ON IT'S PLACE

	memset(bitmap, FRAME_TYPE_ALLOCATED, bitmap_bytesize);	// Fill all of the bits in the bitmap to make sure that all of the memory is reserved by default
	usedRAM += PAGE_SIZE * 8 * bitmap_bytesize;

	for (size_t i = 0; bootInfo->mmap[i].flags & MEMINFO_FLAG_PRESENT; i++)	/* Unlock bitmap entries for all of the free entries in the memory map */
	{
		if (bootInfo->mmap[i].area_type == MEMMAP_AREA_TYPE_USABLE)
		{
			if (unlockRAM((void*)bootInfo->mmap[i].start_address, (bootInfo->mmap[i].area_size / PAGE_SIZE)) == -1)
				return -1;

			usedRAM -= PAGE_SIZE * 8 * bootInfo->mmap[i].area_size;
			freeRAM += PAGE_SIZE * 8 * bootInfo->mmap[i].area_size;
		}
	}

	if (lockRAM(bitmap, bitmap_bytesize / PAGE_SIZE) == -1)
		return -1;

	return 0;
}

static uint8_t getFrameStatus(int pageNumber)
{
	size_t byteIndex = pageNumber / 8;
	size_t bitIndex = pageNumber % 8;

	if (pageNumber < 8)	// If the page number is less then eight, then it is located in the first byte, and the first byte in the list is under index 0
		byteIndex = 0;

	/*
		First the byte index is found and is used as an index for the bitmap to indicate in which bitmap entry(byte) the bit corresponding to the page is located. Then bit index is found, which represents the bit number in the bitmap entry corresponding to the frame(so it indicates the bit itself). The byte index entry of the bitmap is shifted to the left by bit index so the bit corresponding to the frame is moved to bit 0 position in the byte, and when it is on position 0 in the byte, it is ANDed with 1 which gives 0 if the bit is zero and 1 if 1 as 1 has all bits as zeroes except the first one(in position zero).
	*/
	uint8_t bit_status = (bitmap[byteIndex] >> bitIndex) & 1;

	return bit_status;
}

static int lockRAM(void* frame_address, size_t frame_count)
{
	size_t start_page_number = (address_tt)frame_address / PAGE_SIZE;	// Page number is obtained by diving by page size(which is 4096)
	size_t start_frame_byte_index = start_page_number / 8;	// Find byte index, by dividing by eight as that is the number of bytes to go through to access to this entry as each byte stores statuses for 8 pages in each of it's bits
	if (start_frame_byte_index >= bitmap_bytesize)
		return -1;

	size_t currentBitIndex;

	for (size_t current_page_number = start_page_number, current_frame_byte_index = current_page_number / 8; frame_count > 0 && current_frame_byte_index < bitmap_bytesize; frame_count--, current_page_number++, current_frame_byte_index = current_page_number / 8)
	{
		currentBitIndex = current_page_number % 8;	// Bit index is got by finding the leftover from diving page number by amount of statuses there are in each byte as the left when dividing by 8 could range only from 0 to 7 which is exatly the numbers of all bit positions in one byte

		if (getFrameStatus(current_page_number) == FRAME_TYPE_ALLOCATED)
			return -1;

		/*
			The byte where the frame's status bit is located is ORed with a byte where the only ON bit is located(it is done by shifting 1(where bit 0 is set) to the left the bitIndex amount of times) in the position where the bit representing page status in status byte is located. OR operation is used so that if the bit is already set, it stays set and if it isn't - it is turned ON.
		*/
		bitmap[current_frame_byte_index] |= 1 << currentBitIndex;

		freeRAM -= PAGE_SIZE;
		usedRAM += PAGE_SIZE;
	}

	return 0;
}

static int unlockRAM(void* frame_address, size_t frame_count)
{
	size_t start_page_number = (address_tt)frame_address / PAGE_SIZE;	// Page number is obtained by diving by page size(which is 4096)
	size_t start_frame_byte_index = start_page_number / 8;	// Find byte index, by dividing by eight as that is the number of bytes to go through to access to this entry as each byte stores statuses for 8 pages in each of it's bits
	if (start_frame_byte_index >= bitmap_bytesize)
		return -1;

	size_t currentBitIndex;

	for (size_t current_page_number = start_page_number, current_frame_byte_index = current_page_number / 8; frame_count > 0 && current_frame_byte_index < bitmap_bytesize; frame_count--, current_page_number++, current_frame_byte_index = current_page_number / 8)
	{
		currentBitIndex = current_page_number % 8;	// Bit index is got by finding the leftover from diving page number by amount of statuses there are in each byte as the left when dividing by 8 could range only from 0 to 7 which is exatly the numbers of all bit positions in one byte

		/*
			The byte where the frame's status bit is located is ANDed with a byte where the only ON bit is located(it is done by shifting 1(where bit 0 is set) to the left the bitIndex amount of times) in the position where the bit representing page status in status byte is located. Also ones completement operation is applied to the status byte, it is done as the selected bit in status byte needs to be turned of and not not ON, so in the byte that is ANDed with status one all bits are changed to one and only the selected bit at the same position as status bit in the status byte is turned into zero(as before it was 1 and ones completement changes it to the oppsoite), so the status bit will be certainly turned OFF as ANDing with 0 gives 0
		*/
		bitmap[current_frame_byte_index] &= ~(1 << currentBitIndex);

		freeRAM += PAGE_SIZE;
		usedRAM -= PAGE_SIZE;
	}

	return 0;
}

void* palloc(size_t frame_count)
{
	if (frame_count != 1)	// More then 1 frame is currently unsupported
		return NULL;

	void* return_frame = NULL;

	for (size_t current_page_number = 0; current_page_number < (bitmap_bytesize * 8); current_page_number++)
	{
		if (getFrameStatus(current_page_number) == FRAME_TYPE_FREE && current_page_number != 0) // Second part of && means that the page starting at address zero will never be returned as 0 is also NULL address which should indicate error
		{
			if (lockRAM((void*)(current_page_number * PAGE_SIZE), 1) != 0)
				return NULL;

			return_frame = (void*)(current_page_number * PAGE_SIZE);
			break;
		}
	}

	freeRAM -= PAGE_SIZE;
	usedRAM += PAGE_SIZE;

	if (((address_tt)return_frame % 4096) != 0)
		return NULL;

	return return_frame;
}

int pfree(void* frame_address, size_t frame_count)
{
	if (frame_count != 1)	// More then 1 frame is currently unsupported
		return -1;

	if (unlockRAM(frame_address, frame_count) == -1)
		return -1;

	freeRAM += PAGE_SIZE;
	usedRAM -= PAGE_SIZE;

	return 0;
}

static int debugPMM()
{
	#ifdef PMM_DEBUG
	tosBootProtocol_tt* bootInfo = getBootInfo();
	size_t automatic_mem_size = 0, previous_end = 0;

	for (size_t i = 0; bootInfo->mmap[i].flags & MEMINFO_FLAG_PRESENT; i++)
	{
		if (bootInfo->mmap[i].area_type == MEMMAP_AREA_TYPE_USABLE)
			automatic_mem_size += bootInfo->mmap[i].area_size;

		if (previous_end >= bootInfo->mmap[i].start_address && previous_end != 0)
		{
			debugPrintf("Overlapping AREA - Previous end - %lx, Current Start - %lx\n", previous_end, bootInfo->mmap[i].start_address);
			previous_end = bootInfo->mmap[i].start_address + bootInfo->mmap[i].area_size - 1;
		}
	}

	debugPrintf(
		"Memory Start - %lx, Memory Size - %lx\n"
		"Free RAM Size - %lx, Allocated RAM Size - %lx\n"
		"Kernel Start - %lx, Kernel Size - %lx\n"
		"Free RAM(Calculated with mmap) - %lx\n" // Give the calculation of the free memory by adding up all of the size fields of free areas(automatic in comparasion to one done by PFA)
		, RAMstart, RAMsize, freeRAM, usedRAM, kernel_addr, kernel_size, automatic_mem_size);

	void* page_frame = NULL;
	address_tt previous_page_end = 0, previous_page_start = 0;

	while (1)
	{
		page_frame = palloc(1);

		if (((uintptr_t)page_frame % 4096) != 0)	/* Check if frame is not aligned */
		{
			debugPrintf("Page not aligned - %lx\n", page_frame);
			break;
		}

		if (previous_page_end < (address_tt)page_frame)	/* Check if current page overlaps with previous */
			; // NO COLLISION
		else if (previous_page_start > ((address_tt)page_frame + 4096 - 1))
			; // NO COLLISION
		else
		{
			debugPrintf("Page started inside previous page: Previous End - %lx, New Start - %lx\n", previous_page_end, (address_tt)page_frame);
			break;
		}

		debugPrintf("Page Allocated Successfully - %lx\n", (address_tt)page_frame);

		previous_page_start = (address_tt)page_frame;
		previous_page_end = FIND_END((address_tt)page_frame, PAGE_SIZE);
	}
	if (page_frame == NULL)
		debugPrintf("Reached end of palloc\n");

	// ALSO MAKE SURE THAT NONE OF THE PAGES IS FROM THE RESERVED MEMORY AREA - CHECK RANDOM ADDRESSES FROM THE ONES THAT WERE PRINTED OUT AND MAKE MAKE SURE THAT THEY ARE NOT IN THE RESERVED MEMORY
	#endif

	return 0;
}

int initPMM(void*** bitmap_addr, size_t* bitmap_size)
{
	if (getRAMinfo() == -1)
		return -1;

	if (initFrameTracker() == -1)
		return -1;

	debugPMM();

	*bitmap_addr = (void**)&bitmap;
	*bitmap_size = bitmap_bytesize;

	return 0;
}