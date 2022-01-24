/*
    @author 	= eltertrias,
    @brief 		= kernel library for outputting to ports and screen
	@original 	= https://github.com/mpredfearn/simple-printf
	@license	= BSD 3-Clause
		Copyright (c) 2016, Matt Redfearn
		All rights reserved.

		Redistribution and use in source and binary forms, with or without
		modification, are permitted provided that the following conditions are met:

		* Redistributions of source code must retain the above copyright notice, this
		list of conditions and the following disclaimer.

		* Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		* Neither the name of simple-printf nor the names of its
		contributors may be used to endorse or promote products derived from
		this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
		IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
		FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
		DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
		SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
		OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "kstdlib/iolibk.h"
#include <stddef.h>
#include <stdarg.h>


#ifdef PRINTF_NO_LONG_LONG
typedef long long_long_int;
typedef unsigned long ulong_long_int;
#else
typedef long long long_long_int;
typedef unsigned long long ulong_long_int;
#endif
#define PRINT_BUF_LEN 64
enum flags {
	PAD_ZERO	= 1,
	PAD_RIGHT	= 2,
};


static void outputchar(char **str, char c)
{
	if (str) {
		**str = c;
		++(*str);
	} else {
		kputchar(c);
	}
}

static int prints(char **out, const char *string, int width, int flags)
{
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (flags & PAD_ZERO)
			padchar = '0';
	}
	if (!(flags & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			outputchar(out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		outputchar(out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		outputchar(out, padchar);
		++pc;
	}

	return pc;
}

static int outputi(char **out, long_long_int i, int base, int sign, int width, int flags, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	char *s;
	int t, neg = 0, pc = 0;
	ulong_long_int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, flags);
	}

	if (sign && base == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % base;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= base;
	}

	if (neg) {
		if( width && (flags & PAD_ZERO) ) {
			outputchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, flags);
}

static int vsprintf(char **out, char *format, va_list ap)
{
	int width, flags;
	int pc = 0;
	char scr[2];
	union {
		char c;
		char *s;
		int i;
		unsigned int u;
		long li;
		unsigned long lu;
		long_long_int lli;
		ulong_long_int llu;
		short hi;
		unsigned short hu;
		signed char hhi;
		unsigned char hhu;
		void *p;
	} u;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = flags = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				flags = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				flags |= PAD_ZERO;
			}
			if (*format == '*') {
				width = va_arg(ap, int);
				format++;
			} else {
				for ( ; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}
			switch (*format) {
				case('d'):
					u.i = va_arg(ap, int);
					pc += outputi(out, u.i, 10, 1, width, flags, 'a');
					break;

				case('u'):
					u.u = va_arg(ap, unsigned int);
					pc += outputi(out, u.u, 10, 0, width, flags, 'a');
					break;

				case('x'):
					u.u = va_arg(ap, unsigned int);
					pc += outputi(out, u.u, 16, 0, width, flags, 'a');
					break;

				case('X'):
					u.u = va_arg(ap, unsigned int);
					pc += outputi(out, u.u, 16, 0, width, flags, 'A');
					break;

				case('c'):
					u.c = va_arg(ap, int);
					scr[0] = u.c;
					scr[1] = '\0';
					pc += prints(out, scr, width, flags);
					break;

				case('s'):
					u.s = va_arg(ap, char *);
					pc += prints(out, u.s ? u.s : "(null)", width, flags);
					break;
				case('l'):
					++format;
					switch (*format) {
						case('d'):
							u.li = va_arg(ap, long);
							pc += outputi(out, u.li, 10, 1, width, flags, 'a');
							break;

						case('u'):
							u.lu = va_arg(ap, unsigned long);
							pc += outputi(out, u.lu, 10, 0, width, flags, 'a');
							break;

						case('x'):
							u.lu = va_arg(ap, unsigned long);
							pc += outputi(out, u.lu, 16, 0, width, flags, 'a');
							break;

						case('X'):
							u.lu = va_arg(ap, unsigned long);
							pc += outputi(out, u.lu, 16, 0, width, flags, 'A');
							break;
#ifndef PRINTF_NO_LONG_LONG
						case('l'):
							++format;
							switch (*format) {
								case('d'):
									u.lli = va_arg(ap, long long);
									pc += outputi(out, u.lli, 10, 1, width, flags, 'a');
									break;

								case('u'):
									u.llu = va_arg(ap, unsigned long long);
									pc += outputi(out, u.llu, 10, 0, width, flags, 'a');
									break;

								case('x'):
									u.llu = va_arg(ap, unsigned long long);
									pc += outputi(out, u.llu, 16, 0, width, flags, 'a');
									break;

								case('X'):
									u.llu = va_arg(ap, unsigned long long);
									pc += outputi(out, u.llu, 16, 0, width, flags, 'A');
									break;

								default:
									break;
							}
							break;
#endif
						default:
							break;
					}
					break;
				case('h'):
					++format;
					switch (*format) {
						case('d'):
							u.hi = va_arg(ap, int);
							pc += outputi(out, u.hi, 10, 1, width, flags, 'a');
							break;

						case('u'):
							u.hu = va_arg(ap, unsigned int);
							pc += outputi(out, u.lli, 10, 0, width, flags, 'a');
							break;

						case('x'):
							u.hu = va_arg(ap, unsigned int);
							pc += outputi(out, u.lli, 16, 0, width, flags, 'a');
							break;

						case('X'):
							u.hu = va_arg(ap, unsigned int);
							pc += outputi(out, u.lli, 16, 0, width, flags, 'A');
							break;

						case('h'):
							++format;
							switch (*format) {
								case('d'):
									u.hhi = va_arg(ap, int);
									pc += outputi(out, u.hhi, 10, 1, width, flags, 'a');
									break;

								case('u'):
									u.hhu = va_arg(ap, unsigned int);
									pc += outputi(out, u.lli, 10, 0, width, flags, 'a');
									break;

								case('x'):
									u.hhu = va_arg(ap, unsigned int);
									pc += outputi(out, u.lli, 16, 0, width, flags, 'a');
									break;

								case('X'):
									u.hhu = va_arg(ap, unsigned int);
									pc += outputi(out, u.lli, 16, 0, width, flags, 'A');
									break;

								default:
									break;
							}
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		else {
out:
			outputchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	return pc;
}

int kprintf(char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsprintf(NULL, fmt, ap);
	va_end(ap);

	return r;
}

void printsys(char* string, int status)
{
	REMOVE_UNUSED_WARNING(status);
	REMOVE_UNUSED_WARNING(string);
	if (status == PRINTSYS_STATUS_SUCCESS)
	{
		setColor(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
		kprintf("[ ");
		setColor(0x00, 0x80, 0x00, 0x00, 0x00, 0x00);
		kprintf("OK");
		setColor(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
		kprintf(" ] ");
	}
	else if (status == PRINTSYS_STATUS_FAIL)
    {
        setColor(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
        kprintf("[ ");
        setColor(0xFF, 0x00, 0x00, 0x00, 0x00, 0x00);
        kprintf("FAIL");
        setColor(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
        kprintf(" ] ");
    }

    setColor(0x00, 0x80, 0x00, 0x00, 0x00, 0x00);
    kprintf("%s", string);
}

// int sprintf(char *buf, char *fmt, ...)
// {
// 	va_list ap;
// 	int r;

// 	va_start(ap, fmt);
// 	r = vsprintf(&buf, fmt, ap);
// 	va_end(ap);

// 	return r;
// }