<!---
	@author 	= eltertrias, Taken from SkiftOS
	@brief 		= includes the description of coding style used in the OS
	@original	= https://github.com/skiftOS/skift/blob/main/manual/00-meta/coding_style.md
	@license 	= Unspecified
		Copyright © 2018-2021 N. Van Bossuyt & contributors

		Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--->



## File Layout
* File sections should be arranged in the following order
1. File Description, *Example:*
	*	```C
		/*
    		@author : nickname
   			@brief : file description
		*/
		```
2. Headers
    * Global
    * Local
3. Macros
4. Types
5. Global Variables
6. Functions
- Each seaction(e.g. file description, declarations, functions, macros...) should be seperated with 2 lines while the elements(like one function from another) within it should be seperated with one line or no lines. *Example:*
	*	```C
		int foo; // Section 1


		int foobar(); // Section 2

		int foofoo(); // Section 2, element 2
		```



## Indentation
* Use 4 spaces as tab.
* **DO NOT** use space as an indentation
* Try no to leave white spaces at the end of lines
* Do not put multiple assignments or statements on a single line

- *Example:*

	* **DON'T**
		```C
			if (condition) do_this;
			do_something_everytime;

			something; something;
		```
	* **DO**
		```C
			if (condition)
				do this;
				do_something_everytime;

			something;
			something;
		```



## Blocks
* `{` and `}` are on their own lines, except on `do while` statements. *Example:*
	* 	```C
			if (cond)
			{
				// ...
			}
			else
			{
				// ...
			}

			do
			{
				// ...
			} while (cond);
		```
* Do not unnecessarily use braces where a single statement will do, unless only one brach of conditional statement is single statement, in latter case use braces in both branches. *Example:*
	*	```C
			if (condition)
				action();

			if (condition)
			{
				do_this();
				do_that();
			}
			else
			{
				otherwise();
			}
		```



## Spaces
* Use spaces after `if`, `for`, `while`, `switch`, `case`, `do`
* Do not use spaces after `sizeof`
* Do not use space after the opening ( and before the closing )
* Use one space on each side of `=  +  -  <  >  *  /  %  |  &  ^  <=  >=  ==  !=  ?  :`
* Do not use space after unary `&  *  +  -  ~  !  sizeof  typeof  alignof  __attribute__  defined`, `++  --` or `. ->`



## Comments
* Put all of the code description near the code and not above or below it, except for header files where comments could be located anywhere or comments that describe algorithms *Example:*
	* If the comment talks about multiple lines use /* */
		*	```c
			int this;				/* Declares a variable for use as a condition and prints hello */
			if (this)
				printf("hello");
	* If the comment talks about one line of code use //
		*	```c
			int this; 				// A variable declaration
			```
	*	```c
			while (condition)	// This does something <-- CORRECT COMMENT
				if (this == this)
					do_something();

			// This does something	<-- WRONG COMMENT
			while (condition)
				if (this == this)
					do_something();
		```
* Use the special formating for the function comments and keep all function comments in the header file. *Example:*
	* 	```C
			/*
				@brief : function description
				@param parameter-name : parameter description
				@param paramter-name2 : parameter description
				@return : return values of the function
			*/
		```

* If the comment describes some smart(hard to understand)/special algorithm the comment could be located above the code and should use /* */ comments
	*	```c
			/*
				A lot of algorithm explanation
				...
			*/
			int this
			for (i = 0; i < 9, i++)
				printf("%d", this);
		```



## Functions
* Do not use more then 10 local variables
* All functions that would be used globally should be seperated from local in a seperate header file and be declared as extern
* All local functions should be declared static
* Return type and modifiers on the same line as the function name.
* *Example:*
	* 	```C
		static void foo()
		{
			bar;
		}
		```



## Headers
* All headers must include a header guard e.g.
	* 	```c
			#ifndef HEADER_NAME_TYPE_H
			#define HEADER_NAME_TYPE_H
			// All of the header contents
			#endif
		```
	* Global(headers that are included by files other then the .c file corresponding to the header) header's defines should look like e.g. `VMM_GLOBAL_H`, while local(headers only included by the corresponding .c file and that have only static functions) should be `VMM_LOCAL_H`

* All of the following must not be in .c files, but in .h files - `struct defines`, `macros`, `function prototypes`, `typedefs`