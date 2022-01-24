/*
	@author = eltertrias
	@brief = includes for the x86_64 interrupts
*/


#ifndef INTERRUPTS_X86_64_DEFINES_H
#define INTERRUPTS_X86_64_DEFINES_H


#define GDT_CODE_SEGMENT 	0x8
#define INTERRUPTS_AMOUNT 	256


#include <stdint.h>
#include <kernel/interrupts.h>
#include <kernel/init.h>


struct IDTentry
{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint16_t type_attr; // type and attributes
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero;     // reserved
} __attribute__((__packed__));

struct IDTdesc
{
	uint64_t base;
    uint16_t limit;
} __attribute__((__packed__));

extern void ASMloadIDT(void* addr);
#endif