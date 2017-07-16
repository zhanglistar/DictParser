/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file comp_data_t_handler.cpp
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

#include "comp_data_t_handler.h"
#include <cstdlib>

ERR_CODE comp_data_t_handler(void *dest,
                             int *dest_size,
                             const void *src,
                             int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(struct comp_data_t) || src_len <= 0),
        "param error!");

    struct comp_data_t *ptr = (struct comp_data_t *) dest;

    char *str1 = NULL;
    char *str2 = NULL;
    str1 = strtok_r((char *)src, ",", &str2);
    CHECK_PTR_NULL(str1);
    ptr->a = std::atoi(str1);
    str1 = strtok_r(NULL, ",", &str2);
    CHECK_PTR_NULL(str1);
    ptr->b = std::atof(str1);
    str1 = strtok_r(NULL, ",", &str2);
    CHECK_PTR_NULL(str1);
    if (strlen(str1) + 1 > sizeof(ptr->c))
    {
        ERRLOG("string too long[%d/%d]!", strlen(str1)+1, sizeof(ptr->c));
        return RET_WRONG;
    }
    memcpy(ptr->c, str1, sizeof(ptr->c));

    return RET_RIGHT;
}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
