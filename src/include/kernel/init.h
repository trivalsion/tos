/*
    @author = eltertrias
    @brief = global defines for hardware platform initialization code
*/


#ifndef INIT_H
#define INIT_H


#include <kstdlib/kernel_types.h>


/* @brief : information about an area in the memory map */
struct mem_info
{
    uint8_t flags;
	#define MEMINFO_FLAG_PRESENT       	0x1 // If the flag is off, this is this entry is the last one and the memory map ended

	address_tt start_address;
	address_tt area_size;

    uint32_t area_type;
	#define MEMMAP_AREA_TYPE_USABLE     0x1
	#define MEMMAP_AREA_TYPE_RESERVED   0x2
	#define MEMMAP_AREA_TYPE_KERNEL		0x3
};

/* @brief : information about the firmware provided framebuffer */
struct framebuffer_info
{
	uint32_t flags;
	#define FRAMEBUFFER_INFO_FLAG_PRESENT 0x1 // If this flag is not set - there is no framebuffer information

    address_tt framebuffer_addr;    // Physical address of the framebuffer
    uint16_t framebuffer_width;   	// Width and height in pixels
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;   	// Pitch in bytes
    uint16_t framebuffer_bpp;     	// Bits per pixel(Depth)
    uint8_t  red_mask_size;       	/* RGB mask sizes and left shifts */
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
	uint8_t  memory_model;        	// Unused
};

/* @brief : information about where the kernel should be mapped in the virtual memory */
struct load_info
{
	uint32_t flags;
	#define LOAD_INFO_FLAG_PRESENT 0x1 // If this flag is not set - there is no load information

	address_tt kernel_load_vaddr;			// Where the kernel should be loaded in the virtual address space, this address should be equal to or above <MAX_ADDR - 2GB>(as the kernel area is the last two gigabytes)
	address_tt extra_data_load_paddr;		// If extra data should be mapped(like the firsr 1MB for bios on x86) below the kernel, it's physical start address should be specified here(CURRENTLY NOT IMPLEMENTED)
	size_t extra_data_load_page_count;		// The size of extra data in pages, if 0 - no extra data should be loaded below the kernel(CURRENTLY NOT IMPLEMENTED)
};

/* @brief : all the informations necessary for kernel to boot */
struct tos_boot_protocol
{
	uint32_t flags;
	#define BOOT_PROTOCOL_FLAG_PRESENT 0x1 	// If set - structure could be read, if off - error occured and an empty struct was returned

	struct framebuffer_info fb_info;

	#define MMAP_MAX_SIZE 131
	struct mem_info mmap[MMAP_MAX_SIZE];
	struct load_info vla_info;
};
typedef struct tos_boot_protocol tosBootProtocol_tt;


/*
    @breif = reads needed information from the bootloader based on command and responds with it
	@param functions = selects the function that the bootloader should perform
	@param data = unspecified pointer(void*) to some data returned by function(can point to nothing)
	@return = 0 on success, -1 on fail
*/
extern tosBootProtocol_tt arch_bootloaderInterface(void);

/*
    @brief = main function of the kernel that calls all other functions
*/
extern void kernel_main(void);

/*
	@brief = setups the stuff needed(if there is some) on the okatform by the kernel
*/
extern void kernel_setup(void);
#endif