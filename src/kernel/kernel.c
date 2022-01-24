/*
	@author = eltertrias
	@brief = temporary kernel code for testing the kernel in long mode without interfering with the 32 bit kernel version(so it could also be ran)
*/


#include "include/kernel.h"

#include "../drivers/serial.c" // hacky approach until device manager is implemented


static tosBootProtocol_tt bootInfo;
static tosBootProtocol_tt bootInfoCopy; // A copy whos address is given to drivers so they can't overwrite the main one;


void kernel_main()
{
	void** pmm_data_addr;
	size_t pmm_data_size;

	// Call the bootloader function to initialize whatever it needs and return the boot information
	if ((bootInfo = arch_bootloaderInterface()).flags & BOOT_PROTOCOL_FLAG_PRESENT)
		;
	else
		while (1)
			;

	/* DEBUG CODE */
	initSerial();
	printSerial("\n\n---------------------------------------------------------------------------------------\n");

	initPMM(&pmm_data_addr, &pmm_data_size);
	initVMM(pmm_data_addr, pmm_data_size);

	while(1)
		;
}

tosBootProtocol_tt* getBootInfo()
{
	memcpy(&bootInfoCopy, &bootInfo, sizeof(bootInfoCopy));

	return &bootInfoCopy;
}