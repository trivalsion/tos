/*
	@author : eltertrias
	@brief : includes config macros for the vmm
*/


#define CPU_BITNESS 						64

#define ADDRESS_PAGE_OFFSET_BITSIZE			12		// Amount of bits used in the address for offset in a page(4096 byte)
#define ADDRESS_PAGE_TABLE_OFFSET_BITSIZE	9		// Amount of bits used in the address for offset in one of the page tables
#define ADDRESS_PAGE_TABLE_OFFSET_BITMASK 	0x1FF	// Bitmasl to get the bits corresponding to "offset in a page table 'n'" from the address
#define ADDRESS_PAGE_OFFSET_BITMASK			0xFFF	// Bitmask to get the bits corresponding to "byte in a page" from the address
#define USABLE_ADDRESS_BITS 				48

#define PAGE_TABLE_LEVELS_AMOUNT 			4
#define TWO_GB								0x80000000
#define SIZE_LEVEL_4_PAGE_TABLE 			0x1000000000000	// 256 TiB
#define SIZE_LEVEL_3_PAGE_TABLE				0x8000000000	// 512 GiB
#define SIZE_LEVEL_2_PAGE_TABLE				0x40000000		// 1 GiB
#define SIZE_LEVEL_1_PAGE_TABLE				0x200000		// 2 MiB

#define KERNEL_FREE_SPACE_VADDR 	(UINTPTR_MAX - TWO_GB + 1)	// The start of the kernle data, grows downwards
#define USER_FREE_SPACE_VADDR		0x0	// Start of user data - grows upwards