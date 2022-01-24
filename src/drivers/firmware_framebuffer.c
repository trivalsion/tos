/*
	@author = eltertrias
	@brief = a driver for using the framebuffer supplied by firmware like UEFI or BIOS
*/


#include "include/firmware_framebuffer.h"
#include "kernel/iolibk.h"

#include <kernel/libk_debug.h>


static struct framebuffer_info fb_info;
static void* framebuffer;
static size_t framebuffer_size;
static void* framebuffer_end_addr;

static uint32_t global_text_fg_color;		/* Vars specifying the color of next char and it's background */
static uint32_t global_text_bg_color;
static size_t global_text_pos_x;			/* Variables specifying the x and y at which next letter will be drawn */
static size_t global_text_pos_y;


static int drawPSF(uint8_t font_height, uint8_t letter, uint32_t color)
{
	if (letter > 256-1)		// If the letter is bigger then last glyph in array - fail
		letter = 0;

	uint8_t font_width = 8;
	size_t mask = 256;

	size_t i = 0;
	size_t j = 0;

	size_t saved_start_pos_x = global_text_pos_x;

	while (i < font_height)
	{
		if (glyphs[letter][i] & mask)	// Get the status of the current bit(which represents current pixel), if 1 set it to fg color, else to bg color
			*(uint32_t*)((uintptr_t)framebuffer + fb_info.framebuffer_pitch * global_text_pos_y + (fb_info.framebuffer_bpp / 8) * global_text_pos_x) = color;
		else
			*(uint32_t*)((uintptr_t)framebuffer + fb_info.framebuffer_pitch * global_text_pos_y + (fb_info.framebuffer_bpp / 8) * global_text_pos_x) = global_text_bg_color;

		if (j < font_width)
		{
			mask /= 2;
			j++;
			global_text_pos_x++;
		}
		else
		{
			global_text_pos_x = saved_start_pos_x;
			global_text_pos_y++;
			i++;
			j = 0;
			mask = 256;
		}
	}

	global_text_pos_x = global_text_pos_x + font_width+1;
	global_text_pos_y = global_text_pos_y - font_height;

	return 0;
}

static int drawChar(uint8_t letter)
{
	if (global_text_pos_x >= fb_info.framebuffer_width || global_text_pos_y >= fb_info.framebuffer_height)
		return -1;

	if (letter == '\t')
		for (int i = 0; i < 4; i++)
			drawPSF(16, ' ', global_text_fg_color);
	else if (letter == '\n')
	{
		global_text_pos_x = 0;
		global_text_pos_y += 16;
	}
	else
		drawPSF(16, letter, global_text_fg_color);

	if (global_text_pos_x >= fb_info.framebuffer_width || global_text_pos_x > fb_info.framebuffer_width-8) /* IF PROBLMS OCCUR WITH SOME SYMBOLS GETTING FROM END (WHEN THERE IS NOT ENOUGH SPACE TO PINT IT ON CURRENT LINE)TO START CHANGE 8 to 9*/
	{
		global_text_pos_x = 0;
		global_text_pos_y += 16;
	}

	return 0;
}

// static int drawString(char* string)
// {
// 	for (size_t i = 0; string[i] != 0; i++)
// 		drawChar(string[i]);
// 	return 0;
// }

int setColor(uint8_t fg_red, uint8_t fg_green, uint8_t fg_blue, uint8_t bg_red, uint8_t bg_green, uint8_t bg_blue)
{
	global_text_fg_color = (uint32_t)(fg_red << fb_info.red_mask_shift) | (uint32_t)(fg_green << fb_info.green_mask_shift) | (uint32_t)(fg_blue << fb_info.blue_mask_shift); // Put each color at it's offset in bytes taken by pixel and then combine them into one variable of uint32_t as it is normal amount of bites per pixel
	global_text_bg_color = (uint32_t)(bg_red << fb_info.red_mask_shift) | (uint32_t)(bg_green << fb_info.green_mask_shift) | (uint32_t)(bg_blue << fb_info.blue_mask_shift);

	return 0;
}

int putPixel(size_t pos_x, size_t pos_y, uint8_t red, uint8_t green, uint8_t blue)
{
	if (pos_x >= fb_info.framebuffer_width || pos_y >= fb_info.framebuffer_height)	// Make sure the pixel is not beyond framebuffer
		return -1;

	uint32_t pixel = (uint32_t)(red << fb_info.red_mask_shift) | (uint32_t)(green << fb_info.green_mask_shift) | (uint32_t)(blue << fb_info.blue_mask_shift); // Put each color at it's offset in bytes taken by pixel and then combine them into one variable of uint32_t as it is normal amount of bites per pixel

	*(uint32_t*)((uintptr_t)framebuffer + pos_y * fb_info.framebuffer_pitch + pos_x * (fb_info.framebuffer_bpp / 8)) = pixel;

	return 0;
}

void cleanScreen()
{
	size_t i;

	for (i = 0; i < (fb_info.framebuffer_height * fb_info.framebuffer_pitch); i++)	// Each pixel is 32 bits in size so just loop through the framebuffer whose address is converted to a uint32_t poitner and set all bytes to the background color
		((uint32_t*)framebuffer)[i] = global_text_bg_color;

	global_text_pos_x = 0;
	global_text_pos_y = 0;
}

int framebufferInit()
{
	uint8_t* current_shift_var_address;

	if (getBootInfo()->fb_info.flags & FRAMEBUFFER_INFO_FLAG_PRESENT)
		memcpy(&fb_info, &(getBootInfo()->fb_info), sizeof(fb_info));

	#ifdef FIRMWARE_FRAMEBUFFER_DEBUG
	debugPrintf("Address - %lx\n", fb_info.framebuffer_addr);
	debugPrintf("Depth - %lu\n", fb_info.framebuffer_bpp);
	debugPrintf("Height - %lu\n", fb_info.framebuffer_height);
	debugPrintf("Width - %lu\n", fb_info.framebuffer_width);
	debugPrintf("Pitch - %lu\n", fb_info.framebuffer_pitch);
	debugPrintf("Blue mask shift - %d\n", fb_info.blue_mask_shift);
	debugPrintf("Blue mask size - %d\n", fb_info.blue_mask_size);
	debugPrintf("Red mask shift - %d\n", fb_info.red_mask_shift);
	debugPrintf("Red mask size - %d\n", fb_info.red_mask_size);
	debugPrintf("Green mask shift - %d\n", fb_info.green_mask_shift);
	debugPrintf("Green mask size - %d\n", fb_info.green_mask_size);
	#endif

	framebuffer = (void*)fb_info.framebuffer_addr;
	framebuffer_size = fb_info.framebuffer_height * fb_info.framebuffer_pitch * (fb_info.framebuffer_bpp / 8);
	framebuffer_end_addr = (void*)((uintptr_t)framebuffer + framebuffer_size - 1);

	for (int i = 0; i < 3; i++) /* Shift is specified from the first bit here -> 0x0, but on x86 the first bit is here 0x0<-. So to avoid complex shift operations they are just converted so the first bit is where it is on x86*/
	{
		if (i == 0)
			current_shift_var_address = &fb_info.red_mask_shift;
		else if (i == 1)
			current_shift_var_address = &fb_info.green_mask_shift;
		else if (i == 2)
			current_shift_var_address = &fb_info.blue_mask_shift;

		if (*current_shift_var_address == 16)
			*current_shift_var_address = 0;
		else if (*current_shift_var_address == 0)
			*current_shift_var_address = 16;
	}

	setColor(0x00, 0x80, 0x00, 0x00, 0x00, 0x00);
	cleanScreen();

	return 0;
}

int kputchar(int chara)
{
	return drawChar(chara);
}