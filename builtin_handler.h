// handler declearation
#ifndef __CODE_MASTER_BUILTIN_HANDLER_H__
#define __CODE_MASTER_BUILTIN_HANDLER_H__

#include "parser.h"

// builtin type handlers
ERR_CODE int_handler(void *dest, int , const void *src, int );
ERR_CODE float_handler(void *dest, int , const void *src, int );
ERR_CODE string_handler(void *dest, int , const void *src, int );
ERR_CODE double_handler(void *dest, int , const void *src, int );

// builtin type array handlers
ERR_CODE int_array_handler(void *dest, int , const void *src, int );
ERR_CODE float_array_handler(void *dest, int , const void *src, int );
ERR_CODE string_array_handler(void *dest, int , const void *src, int );
ERR_CODE double_array_handler(void *dest, int , const void *src, int );

#endif
/*  vim: set ts=4 sw=4 sts=4 tw=100 */
