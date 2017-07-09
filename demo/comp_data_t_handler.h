/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file comp_data_t_handler.h
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

#ifndef GOODCODER_CHENMING05_DEMO_COMP_DATA_T_HANDLER_H
#define GOODCODER_CHENMING05_DEMO_COMP_DATA_T_HANDLER_H

#include "dict_parser.h"

// user define type
struct comp_data_t
{
    int a;
    double b;
    char c[10];
};

// user define type handler
ERR_CODE comp_data_t_handler(void *dest, int *dest_size, const void *src, int src_len);

#endif

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
