/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file parser.h
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

// handler declearation
#ifndef GOODCODER_CHENMING05_BUILTIN_HANDLER_H
#define GOODCODER_CHENMING05_BUILTIN_HANDLER_H

#include "dict_parser.h"

// builtin type handlers
ERR_CODE int_handler(void *dest, int *dest_size, const void *src, int src_len);
ERR_CODE float_handler(void *dest, int *dest_size, const void *src, int src_len);
ERR_CODE string_handler(void *dest, int *dest_size, const void *src, int src_len);

// builtin type array handlers
ERR_CODE int_array_handler(void *dest, int *dest_size, const void *src, int src_len);
ERR_CODE float_array_handler(void *dest, int *dest_size, const void *src, int src_len);
ERR_CODE string_array_handler(void *dest, int *dest_size, const void *src, int src_len);

#endif
/*  vim: set ts=4 sw=4 sts=4 tw=100 */
