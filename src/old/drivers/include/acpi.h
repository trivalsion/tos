/*
  @author = eltertrias
  @brief = ACPI hardware detection driver global header
*/


#ifndef ACPI_H
#define ACPI_H


#include <stdint.h>


#define ACPI_CONTROL_FIND_FADT 1

#define MADTsignature "APIC"
#define BGRTsignature "BGRT"
#define BERTsignature "BERT"
#define CPEPsignature "CPEP"
#define DSDTsignature "DSDT"
#define ECDTsignature "ECDT"
#define EINJsignature "EINJ"
#define ERSTsignature "ERST"
#define FADTsignature "FACP"
#define FACSsignature "FACS"
#define HESTsignature "HEST"
#define MSCTsignature "MSCT"
#define MPSTsignature "MPST"
#define PMTTsignature "PMTT"
#define PSDTsignature "PSDT"
#define RASFsignature "RASF"
#define SBSTsignature "SBST"
#define SLITsignature "SLIT"
#define SRATsignature "SRAT"
#define SSDTsignature "SSDT"


struct SDTheader
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

struct GenericAddressStructure  // For register position identification
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

struct FADT
{
    struct   SDTheader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

    uint8_t  Reserved;

    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;

    uint8_t  Reserved2;
    uint32_t Flags;

    struct GenericAddressStructure ResetReg;

    uint8_t  ResetValue;
    uint8_t  Reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;

    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
};

/*
  @brief = searches for correct structure within the saved ones and copies it to user specified buffer if it was found and fits
  @param buf = the buffer to where the data structure should be copied
  @param size = the size of the buffer to which it should be copied
  @param signature = the signature of struct that should be searched for
  @return = 0 on success, -1 on fail
*/
int retrieveSDT(void* buf, int size, char* signature);

/*
  @brief = main function to control ACPI table searching
  @return = 0 on success, -1 on fail
*/
extern int ACPIinit(void);
#endif