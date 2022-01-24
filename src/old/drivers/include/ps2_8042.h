/*
    @author = eltertrias
    @breif = global header for 8042 PS/2 controller driver
*/


#ifndef PS2_8042_H
#define PS2_8042_H


#include <kernel_types.h>


// Ports
#define CONTROLLER_STATUS_PORT  0x64
#define CONTROLLER_COMMAND_PORT 0x64
#define CONTROLLER_DATA_PORT    0x60

// Controller Commands(For status port)
#define READ_CONFIG_BYTE    0x20        // TODO: Read response byte from 0x60
// 0x21 to 0x3f Read byte N from internal ram
#define WRITE_CONFIG_BYTE   0x60        // TODO: Write next byte to 0x60
// 0x61 to 0x7F	Write next byte to "byte N" of internal RAM (where 'N' is the command byte & 0x1F)
#define DISABLE_PS2_PORT2   0xA7
#define ENABLE_PS2_PORT2    0xA8
#define TEST_PS2_PORT2      0xA9        // Response: 0x00(Passed), 0x01-4(Failed)
#define TEST_PS2_CONTROLLER 0xAA        // Response: 0x55(Passed), 0xFC(Failed)
#define TEST_PS2_PORT1      0xAB        // Response: 0x00(Passed), 0x01-4(Failed)
#define DISABLE_PS2_PORT1   0xAD
#define ENABLE_PS2_PORT1    0xAE
#define INPUT_TO_STATUS     0xC1        // Copy bits 0-3 from input port to 4-7 in status
#define INPUT_TO_STATUS2    0xC2        // Copy bits 4-7 from input port to 4-7 in status
#define READ_CONTROLLER_OUTPUT_PORT     0xD0    // TODO: Read response byte from 0x60
#define WRITE_CONTROLLER_OUTPUT_PORT    0xD1    // TODO: Write next byte to 0x60
#define WRITE_PS2_PORT1_OUTPUT          0xD2    // TODO: Write next byte to 0x60
#define WRITE_PS2_PORT2_OUTPUT          0xD3    // TODO: Write next byte to 0x60
#define WRITE_PS2_PORT2_INPUT           0xD4    // TODO: Write next byte to 0x60
// Existance is up to firmware
#define DIAGNOSTIC_DUMP_UNDEFINED   0xAC
#define READ_INPUT_PORT_UNDEFINED   0xC0

// Status Byte(AND the status byte with needed define to get status)
#define STATUS_OUTPUT_BUFFER            0x01 // Status: 0 - empty, 1 - full
#define STATUS_INPUT_BUFFER             0x02 // 0 - empty, 1 - full
#define STATUS_SYSTEM_FLAG              0x04 // 1 - POST passed, 0 - not passed - Implementation can be buggy in firmware so treat as unknown
#define STATUS_WRITE_TYPE               0x08 // 0 - data written to input buffer is for device, 1 - for controller
#define STATUS_KEYLOCK_UNDEFINED        0x10 // Chipset specific
#define STATUS_RCV_TIMEOUT_UNDEFINED    0x20 // Chipset specific
#define STATUS_TIMEOUT_ERROR            0x40 // 0 - no error, 1 - time-out error
#define STATUS_PARITY_ERROR             0x80 // 0 - no error, 1 - parity error

// Config Byte(AND the status byte with needed define to get status)
#define CONFIG_PS2_PORT1_INTERRUPT      0x01 // Status 1 - interrupt enabled, 0 - disabled
#define CONFIG_PS2_PORT2_INTERRUPT      0x02 // 1 - interrupt enabled, 0 - disabled
#define CONFIG_POST_PASSED              0x04 // 1 - passed POST, 0 - system is broken(POST not passed) - Implementation can be buggy in firmware so treat as unknown
#define CONFIG_ZERO                     0x08 // Should be 0
#define CONFIG_PS2_PORT1_CLOCK          0x10 // 1 - clock off, 0 - on
#define CONFIG_PS2_PORT2_CLOCK          0x20 // 1 - clock off, 0 - on
#define CONFIG_PS2_PORT1_TRANSLATION    0x40 // 1 - enabled, 0 - disabled
#define CONFIG_ZERO2                    0x80 // Should be 0

// Controller Output Port(AND the status byte with needed define to get status)
#define OUTPUT_SYSTEM_RESET         0x01 // 1 - don't reset system, 0 - reset system(can lock up the computer)
#define OUTPUT_A20                  0x02
#define OUTPUT_PS2_PORT2_CLOCK      0x04
#define OUTPUT_PS2_PORT2_DATA       0x08
#define OUTPUT_READ_PS2_PORT1_BYTE  0x10
#define OUTPUT_READ_PS2_PORT2_BYTE  0x20
#define OUTPUT_PS2_PORT1_CLOCK      0x40
#define OUTPUT_PS2_PORT1_DATA       0x80

// IOctl commands
#define IOCTL_CONTROLLER_SEND           0
#define IOCTL_CONTROLLER_RECEIVE        1
#define IOCTL_CONTROLLER_DIRECT_SEND    2
#define IOCTL_CONTROLLER_DIRECT_RECIEVE 3

// Controller Info defenitions
#define DEVICE_PS2_MOUSE                    0x00
#define DEVICE_SCROLL_MOUSE                 0x03
#define DEVICE_5BUTTON_MOUSE                0x04
#define DEVICE_MF2_KEYBOARD_TRANSLATION     0xAB
#define DEVICE_MF2_KEYBOARD_TRANSLATION2    0x41
#define DEVICE_MF2_KEYBOARD_TRANSLATION3    0xC1
#define DEVICE_MF2_KEYBOARD                 0xAB
#define DEVICE_MF2_KEYBOARD1                0x83
#define DEVICE_BROKEN                       0xEE

#define CHANNEL_STATUS_BROKEN   0
#define CHANNEL_STATUS_WORKING  1


/*
    @brief = initialization of the PS/2 controller
    @return = 0 on success, -1 on error
*/
extern int ps2ControllerInit(void);

/*
    @brief = ioctl function to control the 8042
    @param request = The command to send to the controller
    @param ... = Extra parameters that might be required
    @return = 0 or nonnegative on success, -1 on error
*/
extern int ioctl(unsigned long request, ...);


/*
    @brief = Disabled write function
    @param buf = Buffer to write data from
    @param count = Bytes amount to be written from buffer
    @return = -1 on error
*/
extern ssize_t ps2_8042Write(void* buf, size_t count);

/*
    @brief = Disabled read function
    @param buf = Buffer to read data to
    @param count = Bytes amount to be read to the buffer
    @return = -1 on error
*/
extern ssize_t ps2_8042Read(void* buf, size_t count);
#endif