/*
    @author = eltertrias
    @brief = includes code needed to get data and setup everything using stivale2(https://github.com/stivale/stivale/blob/master/STIVALE2.md)
*/


#include "stivale-include.h"


static uint8_t stack[16384];								// 16 KB reserved for the stack
static struct stivale2_tag* stivale2_tags_struct_ptr;
static tosBootProtocol_tt bootInfo;

struct stivale2_header_tag_framebuffer framebuffer_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next       = 0,									// 0 means that it is the end of the linked list of tags
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0,
};

struct stivale2_header __attribute__((section(".stivale2hdr"), used)) header2 = {	// The stivale2 header structure is defined with GCC attributes: section - indicates that the variable should be placed in non-default section chosen by user; used - indicates that the compiler should make the variable even if it is not used anywhere in the code
    .entry_point = 0,										// Entry point should be 0 so the one specified in elf header will be used
    .stack       = (uintptr_t)&stack + sizeof(stack),		// Address of the space that is reserved for the stack
    .flags       = 0,										// All bits are undefined and should be zero
    .tags        = (uint64_t)&framebuffer_request			// Pointer to the first member in a linked list of tags
};


void _start(struct stivale2_struct* info)	/* The function has struct pointer as an argument because stivale protocol puts the pointer to it's info struct into rdi which in "System V AMD64 ABI Calling Convention" is used to store the first argument that is an integer/pointer(not the stack as in i386 ABI) */
{
	stivale2_tags_struct_ptr = (struct stivale2_tag *)info->tags;
	struct stivale2_tag* tag_current;

	for (tag_current = stivale2_tags_struct_ptr; tag_current != NULL; tag_current = (struct stivale2_tag *)tag_current->next)
	{
		switch (tag_current->identifier)
		{
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
                if (readMMAP() == -1)
					return; // RETURN IS IMPOSSIBLE AS KERNEL WAS CALLED BY BOOTLOADER SO IT LOADS TO TRIPLE FAULT
                break;
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
			{
				if (readFramebufferInfo() == -1)
					return;
                break;
			}
            case STIVALE2_STRUCT_TAG_RSDP_ID:
                break;
            default:
                break;
        }
	}

	bootInfo.vla_info.kernel_load_vaddr = 0xffffffff80100000;
	bootInfo.vla_info.extra_data_load_paddr = 0;
	bootInfo.vla_info.extra_data_load_page_count = 0;
	toggleBit((size_t*)&bootInfo.vla_info.flags, LOAD_INFO_FLAG_PRESENT, TOGGLE_BIT_ON);

	kernel_setup();
}

static int readFramebufferInfo()
{
	struct stivale2_tag *tag_current;
	struct stivale2_struct_tag_framebuffer* fb;

	for (tag_current = stivale2_tags_struct_ptr; tag_current != NULL; tag_current = (struct stivale2_tag *)tag_current->next)
	{
		if (tag_current->identifier == STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID)
		{
			fb = (struct stivale2_struct_tag_framebuffer *)tag_current;

			bootInfo.fb_info.framebuffer_addr = fb->framebuffer_addr;
			bootInfo.fb_info.framebuffer_width = fb->framebuffer_width;
			bootInfo.fb_info.framebuffer_height = fb->framebuffer_height;
			bootInfo.fb_info.framebuffer_pitch = fb->framebuffer_pitch;
			bootInfo.fb_info.framebuffer_bpp = fb->framebuffer_bpp;
			bootInfo.fb_info.red_mask_size = fb->red_mask_size;
			bootInfo.fb_info.red_mask_shift = fb->red_mask_shift;
			bootInfo.fb_info.green_mask_size = fb->green_mask_size;
			bootInfo.fb_info.green_mask_shift = fb->green_mask_shift;
			bootInfo.fb_info.blue_mask_size =	fb->blue_mask_size;
			bootInfo.fb_info.blue_mask_shift = fb->blue_mask_shift;

			toggleBit((size_t*)&bootInfo.fb_info.flags, FRAMEBUFFER_INFO_FLAG_PRESENT, TOGGLE_BIT_ON);
		}
	}

	return 0;
}

static int readMMAP()
{
	struct stivale2_tag *tag_current;
	struct stivale2_struct_tag_memmap* mmap;

	for (tag_current = stivale2_tags_struct_ptr; tag_current != NULL; tag_current = (struct stivale2_tag *)tag_current->next)	/* Parse the memory map and put it in the kernels format(proccess described in multiboot 2 spec) */
	{
		if (tag_current->identifier == STIVALE2_STRUCT_TAG_MEMMAP_ID)
		{
			mmap = (struct stivale2_struct_tag_memmap*)tag_current;

			if (mmap->entries > MMAP_MAX_SIZE)
				return -1;

			for (size_t i = 0; i < mmap->entries; i++)
			{
				toggleBit((size_t*)&bootInfo.mmap[i].flags, MEMINFO_FLAG_PRESENT, TOGGLE_BIT_ON);
				bootInfo.mmap[i].start_address = mmap->memmap[i].base;
				bootInfo.mmap[i].area_size = mmap->memmap[i].length;

				if (mmap->memmap[i].type == STIVALE2_MMAP_USABLE)
					bootInfo.mmap[i].area_type = MEMMAP_AREA_TYPE_USABLE;
				else if (mmap->memmap[i].type == STIVALE2_MMAP_KERNEL_AND_MODULES)
					bootInfo.mmap[i].area_type = MEMMAP_AREA_TYPE_KERNEL;
				else
					bootInfo.mmap[i].area_type = MEMMAP_AREA_TYPE_RESERVED;
			}
		}
	}

	return 0;
}

tosBootProtocol_tt arch_bootloaderInterface()
{
	toggleBit((size_t*)&bootInfo.flags, BOOT_PROTOCOL_FLAG_PRESENT, TOGGLE_BIT_ON);
	return bootInfo;
}

static void toggleBit(size_t* var, size_t bitmask, uint8_t bit_status)
{
    if ((bit_status == TOGGLE_BIT_ON && (*var & bitmask)) || (bit_status == TOGGLE_BIT_OFF && !(*var & bitmask)))
        ;
    else if ((bit_status == TOGGLE_BIT_ON && !(*var & bitmask)) || (bit_status == TOGGLE_BIT_OFF && (*var & bitmask)))
        *var ^= bitmask;
}