#ifndef __LOADERS_STIVALE_STIVALE_H__
#define __LOADERS_STIVALE_STIVALE_H__

#include <Base.h>

#pragma pack(1)

typedef struct _STIVALE2_STRUCT {
    CHAR8 BootloaderBrand[64];
    CHAR8 BootloaderVersion[64];
    void* Tags;
} STIVALE2_STRUCT;

typedef struct _STIVALE2_HEADER {
    UINT64 EntryPoint;
    UINT64 Stack;
    UINT64 Flags;
#define STIVALE2_HEADER_FLAG_KASLR BIT0
    void* Tags;
} STIVALE2_HEADER;

typedef struct _STIVALE2_HDR_TAG {
    UINT64 Identifier;
    void* Next;
} STIVALE2_HDR_TAG;

#define STIVALE2_HEADER_TAG_FRAMEBUFFER_IDENT 0x3ecc1bc43d0f7971
typedef struct _STIVALE2_HEADER_TAG_FRAMEBUFFER {
    UINT64 Identifier;
    void* Next;
    UINT16 FramebufferWidth;
    UINT16 FramebufferHeight;
    UINT16 FramebufferBpp;
} STIVALE2_HEADER_TAG_FRAMEBUFFER;

#define STIVALE2_HEADER_TAG_PML5_IDENT 0x932f477032007e8f

#define STIVALE2_HEADER_TAG_SMP_IDENT 0x1ab015085f3273df
typedef struct _STIVALE2_HEADER_TAG_SMP {
    UINT64 Identifier;
    void* Next;
    UINT64 Flags;
#define STIVALE2_HEADER_TAG_SMP_FLAG_X2APIC BIT0
} STIVALE2_HEADER_TAG_SMP;

#define STIVALE2_STRUCT_TAG_CMDLINE_IDENT 0xe5e76a1b4597a781
typedef struct _STIVALE2_STRUCT_TAG_CMDLINE {
    UINT64 Identifier;
    void* Next;
    CHAR8* Cmdline;
} STIVALE2_STRUCT_TAG_CMDLINE;

typedef struct _STIVALE2_MMAP_ENTRY {
    UINT64 Base;
    UINT64 Length;
    UINT32 Type;
#define STIVALE2_USEABLE                1
#define STIVALE2_RESERVED               2
#define STIVALE2_ACPI_RECLAIMABLE       3
#define STIVALE2_ACPI_NVS               4
#define STIVALE2_BAD_MEMORY             5
#define STIVALE2_BOOTLOADER_RECLAIMABLE 0x1000
#define STIVALE2_KERNEL_AND_MODULES     0x1001
    UINT32 Unused;
} STIVALE2_MMAP_ENTRY;

#define STIVALE2_STRUCT_TAG_MEMMAP_IDENT 0x2187f79e8612de07
typedef struct _STIVALE2_STRUCT_TAG_MEMMAP {
    UINT64 Identifier;
    void* Next;
    UINT64 Entries;
    STIVALE2_MMAP_ENTRY Memmap[];
} STIVALE2_STRUCT_TAG_MEMMAP;

#define STIVALE2_STRUCT_TAG_FRAMEBUFFER_IDENT 0x506461d2950408fa
typedef struct _STIVALE2_STRUCT_TAG_FRAMEBUFFER {
    UINT64 Identifier;
    void* Next;
    UINT64 FramebufferAddr;
    UINT16 FramebufferWidth;
    UINT16 FramebufferHeight;
    UINT16 FramebufferPitch;
    UINT16 FramebufferBpp;
    UINT8 MemoryModel;
    UINT8 RedMaskSize;
    UINT8 RedMaskShift;
    UINT8 GreenMaskSize;
    UINT8 GreenMaskShift;
    UINT8 BlueMaskSize;
    UINT8 BlueMaskShift;
} STIVALE2_STRUCT_TAG_FRAMEBUFFER;

typedef struct _STIVALE2_MODULE {
    UINT64 Begin;
    UINT64 End;
    CHAR8 String[128];
} STIVALE2_MODULE;

#define STIVALE2_STRUCT_TAG_MODULES_IDENT 0x4b6fe466aade04ce
typedef struct _STIVALE2_STRUCT_TAG_MODULES {
    UINT64 Identifier;
    void* Next;
    UINT64 ModuleCount;
    STIVALE2_MODULE Modules[];
} STIVALE2_STRUCT_TAG_MODULES;

#define STIVALE2_STRUCT_TAG_RSDP_IDENT 0x9e1786930a375e78
typedef struct _STIVALE2_STRUCT_TAG_RSDP {
    UINT64 Identifier;
    void* Next;
    void* Rsdp;
} STIVALE2_STRUCT_TAG_RSDP;

#define STIVALE2_STRUCT_TAG_EPOCH_IDENT 0x566a7bed888e1407
typedef struct _STIVALE2_STRUCT_TAG_EPOCH {
    UINT64 Identifier;
    void* Next;
    UINT64 Epoch;
} STIVALE2_STRUCT_TAG_EPOCH;

#define STIVALE2_STRUCT_TAG_FIRMWARE_IDENT 0x359d837855e3858c
typedef struct _STIVALE2_STRUCT_TAG_FIRMWARE {
    UINT64 Identifier;
    void* Next;
    UINT64 Flags;
#define STIVALE2_STRUCT_TAG_FIRMWARE_FLAG_BIOS BIT0
} STIVALE2_STRUCT_TAG_FIRMWARE;

typedef struct _STIVALE2_SMP_INFO {
    UINT32 AcpiProcessorUid;
    UINT32 LapicId;
    UINT64 TargetStack;
    UINT64 GotoAddress;
    UINT64 ExtraArgument;
} STIVALE2_SMP_INFO;

#define STIVALE2_STRUCT_TAG_SMP_IDENT 0x34d1d96339647025
typedef struct _STIVALE2_STRUCT_TAG_SMP {
    UINT64 Identifier;
    void* Next;
    UINT64 Flags;
    UINT32 BspLapicId;
    UINT32 Unused;
    UINT64 CpuCount;
    STIVALE2_SMP_INFO SmpInfo[];
} STIVALE2_STRUCT_TAG_SMP;

#pragma pack()

#endif //__LOADERS_STIVALE_STIVALE_H__
