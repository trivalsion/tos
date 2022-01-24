/*
	@author = eltertrias
	@brief = includes various functions for debugging
*/


#ifndef LIBK_DEBUG_GLOBAL_H
#define LIBK_DEBUG_GLOBAL_H


#define REMOVE_UNUSED_WARNING(x) (void)(x) // Removes unused parameter warnings from the supplied variable(it does conversion to nothing and does not assign, meaning it does nothing, but for compiler it is an action with the variable)


/*
	@brief = printf wrapper that outputs to the debug location(e.g. serial port)
*/
int debugPrintf(char *fmt, ...);
#endif