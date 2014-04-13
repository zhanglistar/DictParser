// int handler implementaion

#include "builtin_handler.h"
#include <stdlib.h>

#define CHECK_PARAM(p, q) if (p) { ERRLOG(q); return PARAM_ERROR; }

ERR_CODE int_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(int) || src_len <= 0), 
                "param error!");

    char *pend = NULL;
    *(int *)dest = (int)strtol((char *)src, &pend, 10);

    if (((char *)src)[0] != '\0' && *pend == '\0')
    {
        return RET_RIGHT;
    }
    else
    {
        ERRLOG("format error!");
        return RET_WRONG;
    }
}

ERR_CODE float_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(float) || src_len <= 0), 
                "param error!");
    
    char *pend = NULL;
    *(float *)dest = strtof((const char *)src, &pend);

    if (((char *)src)[0] != '\0' && *pend == '\0')
    {
        return RET_RIGHT;
    }
    else
    {
        return RET_WRONG;
    }
}

ERR_CODE double_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(double) || src_len <= 0), 
                "param error!");
    
    char *pend = NULL;
    *(double *)dest = strtod((char *)src, &pend);

    if (((char *)src)[0] != '\0' && *pend == '\0')
    {
        return RET_RIGHT;
    }
    else
    {
        return RET_WRONG;
    }
}

ERR_CODE string_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < src_len), "param error!");
    
    memcpy(dest, src, src_len);
    return RET_RIGHT;
}

ERR_CODE int_array_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(int) || src_len <= 0), 
                "param error!");
    
    int *pint = (int *)dest;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *) src;
    str1 = strtok_r(pstr, ",", &str2);
    while (str1)
    {
        char *pend = NULL;
        *pint = (int)strtol((char *)str1, &pend, 10);

        if (((char *)src)[0] != '\0' && *pend == '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        ++pint;
        if (pint >= (int *)dest + sizeof(int) * dest_len)
        {
            ERRLOG("buffer not enough");
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    return RET_RIGHT;
}

ERR_CODE float_array_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(float) || src_len <= 0), 
                "param error!");
    
    float *pfloat = (float *)dest;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *) src;
    str1 = strtok_r(pstr, ",", &str2);
    while (str1)
    {
        char *pend = NULL;
        *pfloat = strtof((char *)src, &pend);

        if (((char *)src)[0] != '\0' && *pend == '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        ++pfloat;
        if (pfloat >= (float *)dest + sizeof(float) * dest_len)
        {
            ERRLOG("buffer not enough[%d/%d]", src_len, dest_len);
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    return RET_RIGHT;
}

ERR_CODE string_array_handler(void *dest, int dest_len, const void *src, int src_len)
{
    //char *pstr = (char *) dest;

    return RET_RIGHT;
}

ERR_CODE double_array_handler(void *dest, int dest_len, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < (int)sizeof(double) || src_len <= 0), 
            "param error!");
    
    double *pdouble = (double *)dest;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *) src;
    str1 = strtok_r(pstr, ",", &str2);
    while (str1)
    {
        char *pend = NULL;
        *pdouble = strtod((char *)src, &pend);

        if (((char *)src)[0] != '\0' && *pend == '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        //*pdouble = atof(str1);
        ++pdouble;
        if (pdouble >= (double *)dest + sizeof(double) * dest_len)
        {
            ERRLOG("buffer not enough");
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    return RET_RIGHT;

}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
