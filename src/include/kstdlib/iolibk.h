/*
    @author = eltertrias
    @brief = standard c input/output library header
*/


#ifndef STDIO_H
#define STDIO_H


#define PRINTSYS_STATUS_SUCCESS 0
#define PRINTSYS_STATUS_FAIL -1


#include <kernel/init.h>


/*
	@brief : prints a character to the location specified in kernel config(e.g. display)
	@param chara : specifies the character which should be outputed
	@return: -1 on fail, 0 on success
*/
extern int kputchar(int chara);

/*
	@brief : prints a foramt string to the location specified in kernel config(e.g. display), acts like printf
	@param fmt : string format
	@param ... : variables
	@return: -1 on fail, 0 on success
*/
extern int kprintf(char *fmt, ...);

/*
	@breif : returns the boot information struct
	@return : the pointer to the boot information structure
*/
extern tosBootProtocol_tt* getBootInfo(void);

/*
    @brief = prints a system initialization message
    @param string = the string to print
    @param status = the status of the print - FAIL/SUCCESS
*/
extern void printsys(char* string, int status);
#endif