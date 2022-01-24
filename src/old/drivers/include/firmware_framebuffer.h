/*
	@author = eltertrias
	@brief = global defines for framebuffer driver
*/


#ifndef FIRMWARE_FRAMEBUFFER_H
#define FIRMWARE_FRAMEBUFFER_H


#include <kernel/init.h>


/*
	@brief = initialises everything that framebuffer needs
	@return = -1 on fail, 0 on succes
*/
extern int framebufferInit(void);

/*
	@brief = cleans the screen by overwriting all pixels with background color
*/
extern void cleanScreen(void);

/*
	@brief = changes background and foreground color in RGB
	@param fg_* = specifies the value that the R/G/B should have to change the foreground color
	@param bg_* = specifies the value that the R/G/B should have to change the background color
	@return = 0 on success, -1 on fail
*/
extern int setColor(uint8_t fg_red, uint8_t fg_green, uint8_t fg_blue, uint8_t bg_red, uint8_t bg_green, uint8_t bg_blue);

/*
	@brief = puts a pixel of specified color in x, y spot on screen
	@pos_* = x/y position on the screen
	@last three = value of colors in RGB
*/
extern int putPixel(size_t pos_x, size_t pos_y, uint8_t red, uint8_t green, uint8_t blue);
#endif