# Information
- Description - the framebuffer is just a certain amount of addresses that lead not to RAM, but to VRAM instead. In framebuffer each pixel on screen corresponds to several bytes somewhere in framebuffer(if pixel one it starts at bytes one and so on) which corresponds to where it is on screen(it's number) multiplied by it's size, the bytes in pixel specify it's color in RGB. So by writing into the each bytes(for green, red and blue) you change the color. All of the data about framebuffer should be recieved from the bootloader or firmware.


## Defenitions
* **Width** - amount of pixels on a horizontal line
* **Height** - amount of horizontal lines
* **Depth** - number of bits the pixel takes up in framebuffer(divide by 8 to get bytes)
	* e.g. 8 bits for green, 8 for red and 8 for blue, then depth is `8+8+8=24`
	* Also called BPP(Bits per Pixel) or Pixelwidth
* **Pitch** - amount of bytes in each row(it can be different to width as there may be more bytes at the end that are not used for outputting to the screen, or there could be 32-bit colors...)

* **Red/Green/Blue Shift** - at which bit the number specifying the specific color starts in the bytes that pixel takes up.
	* e.g. if the red shift is 8 then the 255 bits specifing red color in start at bit 8
* **Red/Green/Blue Size** - amount of bits specifying the color


## Formulas
* **Pixels per row** = `pitch / (depth / 8)`
* **Pixel start memory address** = `framebuffer_start_addr + pitch * y + (depth / 8) * x`
* **Framebuffer byte size** = `pitch * height * (depth / 8)`
* **Framebuffer end addr** = `framebuffer_start_addr + framebuffer_byte_size - 1`
* **Combine colors into one variable(work only if depth is 32)** = `(uint32_t)(red << fb_info.red_mask_shift) | (uint32_t)(green << fb_info.green_mask_shift) | (uint32_t)(blue << fb_info.blue_mask_shift)`



# Text Output
## Fonts
- The fonts are of two types
	* Bitmap - these have a fixed size of width * height bits(e.g. 8x8)
	* Vector - these can be adapted to resolution(e.g. larger resolution - larger letters)
- Also the fonts can be represented in different ways of which most common ones are
	* As a C array of height entries with width size
	* As a PSF file which could be read directly into array at runtime or just written as an array at runtime


## PSF
* [Easy PSF Fonts](https://www.zap.org.au/projects/console-fonts-zap/)
* [PSF Spec](https://git.kernel.org/pub/scm/linux/kernel/git/legion/kbd.git/tree/src/psf.h?id=82dd58358bd341f8ad71155a53a561cf311ac974)
* [PSF Usage](https://wiki.osdev.org/PC_Screen_Font)
- Description - PSF fonts are the fonts used by the linux kernel on the PC. These fonts are located in a file of a specified format(there is v1 of the format and the v2). The PSF fonts are just hex byte after byte where each bit means a pixel(1 - ON, 0 - OFF). The first row of pixels in a glyph is the first width bits in the glyp, then when all of these are outputed increase y by 1 and put x to the same position as it was at start of glyph and output the second row of second `width` amount bits and so on until `height` rows were outputted.
### Converting PSF into array at runtime
1. First you should open the file in a hex editor(so it would be easier to copy hex from it)
2. In hex editor you skip the header size bytes at the start of the file and then the glyphs start
3. Now the glyph could be copied - just copy `(height*width) / 8` bytes starting from the first byte after header to get the first glyp then starting from next byte after theis glyph copy next `(height*width) / 8` bytes and so on
4. To put the glyph into array, it should be split into `height` amount of `width / 8` bytes sized chuncks that should be put in reverse order -
	* e.g. if in the PSF file in hex glyph was `88 EE AF 87` and the height is 4 while width is (8 / 8) then they should be in a array of this type
		* 	```C
			uint16_t* glyphs[] = {
				{0x87, 0xAF, 0xEE, 0x88}
			}
			```

### Outputing PSF to framebuffer from array
1. To output the PSF you find a location for the glyph and start outputting
2. Have a bitmask where bit 7 is set and AND it with glyphs[*][0] to get the first pixel status(ON/OFF)
3. If the AND gives 0(OFF), change color of pixel to background color(as it means that this pixel should be empty), otherwsie change the pixel color to forground color
4. Next do the same thing for bit 6,5,4... until(including) the 1(0) bit, then go to the same pos_x location as of the first pixel in the glyph, but on the next row(pos_y + 1).
5. Now output the same amount of bits as before starting from bit 0, but just from the second entry in the glyph glyph[*][1] and so on until the end of the glyph

### Notes
* To start with the first bit in a byte and then go to second... there are two methods
	* Bit shift method
		1. Set the mask - `mask = 1<<(width-1)`
		2. Use the mask - `NUMBER & mask`
		3. Change mask to next bit - `mask >>= 1`
	* Power of two method:
		1. Set the mask - `mask = 1`
		2. Use the mask - `NUMBER & (2^mask)`
		3. Change mask to next bit - `mask++`
	* Multiply by 2 method:
		1. Set the mask - `mask = 1`(bit 0 set)
		2. Use the mask - `NUMBER & mask`
		3. Change mask to next bit - `mask++`



# Research
## Notes
* Pitch(amount of bytes in each row) could be bigger than `width * depth`, as some bytes at the end of row might not lead to screen, so this should be taken into account and several bytes should be sometimes skipped after the `width * depth` to not loose character and go to the next line
* Mostly things like output modes except 32bit one or pitch larger then `width * depth` are deprecated and should not be taken into account unless writing driver for device that has this stuff


## Questions
- How to implement a pixel/font output function that could handle any type of driver?
	* Output functions - there should be a seperate output function for each framebuffer type(they differ in depths) or else these functions may be very slow.
		* There could be one standardized function which write to an C array and the framebuffer specific functions just copy array into the framebuffer memory.
- How could 32 bits be taken if each color is 8 bytes and there are 3 colors?
	* 4th byte is used for alpha channel which is not outputted to the screen but could be usefull to store extra info when writing GUI - that is why it is called RGBA



# Resources
- Tutorials
	* [Outputing text and drawings with framebuffer](https://wiki.osdev.org/Drawing_In_Protected_Mode)
- Implementations
	* [VBE driver, with fonts built-in instead of being loaded](https://github.com/Jacob-C-Smith/GlucOS/blob/master/kernel/source/VBE.c)
	* [UEFI GOP Fraembuffer with PSF fonts](https://www.youtube.com/watch?v=W4rfV6DK0bU)
- Threads
	* [Framebuffer common question answers](https://forum.osdev.org/viewtopic.php?f=1&t=41397)
	* [Reddit OSDev font discussion](https://www.reddit.com/r/osdev/comments/ko6eto/text_font/)