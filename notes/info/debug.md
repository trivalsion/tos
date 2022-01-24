# Debugging Proccess
## Exeption/Interrupt debugging
1. Get interrupts(and exeptions) log file
	1. Start qemu with the following CLI arguments passed to it - `-d int -D file.name`
		* Everytime exeption occures - everytime an interrupt occurs, register dump and exeption info(e.g. vector) is given
		* `v` - exeption vector, `e` - exeption error(located on the stack)
	2. Close qemu immediatly after it fails so next startup produces little-no new exeptions
2. Find the interrupt that caused the reboot(last interrupt)
	1. Get the first line of first interrupt(e.g. `0: v=68 e=0000 i=0 cpl=0 IP=0038:000000000744952a pc=000000000744952a SP=0030:0000000007ef5c40`) and check if it is repeated anywhere in the file. If it is repeated, remove the repeated one and all interrupts after it from the log file as they are from the second boot of the QEMU
	2. Find the last interrupt and start examining it
		* ***Note:*** If it is a double fault(0x08) then check the previous exeption as double fault occures when an exeption occured when there is an already unhandled exeption
3. Find the instruction that caused the interrupt
	1. To find the causes of the exeption - use the dump of IP register(has the address of instruction that was running when exeption occured) and find the instruction that is located on that address in the object file
	2. Use `objdump -d tos.o` to get the object file dump in assembly. Find the address from IP and check which instruction is there




# Extra
## Usefull QEMU Monitor commands
	- `info registers` - dump of current values in registers
	- `info mem` - tells the memory ranges that are mapped somewhere
	- `info tlb` - tells to which physical address each virtual page is mapped(shows page mappings)