// int handler implementaion

#include "builtin_handler.h"
#include <stdlib.h>

ERR_CODE int_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(int) || src_len <= 0),
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

ERR_CODE float_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(float) || src_len <= 0),
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

ERR_CODE string_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(char)), "param error!");

    if (*dest_size < src_len)
    {
        ERRLOG("string too long[%d/%d]!", src_len, *dest_size);
        return LINE_TOO_LONG;
    }
    memcpy(dest, src, src_len);
    return RET_RIGHT;
}

ERR_CODE int_array_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(int) || src_len <= 0),
                "param error!");

    int *pint = (int *)dest;
    int num = -1;
    int i = 0;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *)src;
    str1 = strtok_r(pstr, ":", &str2);
    while (str1)
    {
        char *pend = NULL;
        if (num == -1)
        {
            num = (int)strtol((char *)str1, &pend, 10);
        }
        else
        {
            *(pint + i) = (int)strtol((char *)str1, &pend, 10);
            ++i;
        }

        if (((char *)src)[0] != '\0' && *pend == '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        if (i >= (*dest_size) / (int)sizeof(int))
        {
            ERRLOG("buffer not enough");
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    if (num >= 0 && num == i)
    {
        *dest_size = num * (int)sizeof(int);
        return RET_RIGHT;
    }
    else
    {
        ERRLOG("format error!");
        return RET_WRONG;
    }

}

ERR_CODE float_array_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(float) || src_len <= 0),
                "param error!");

    float *pfloat = (float *)dest;
    int num = -1;
    int i = 0;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *) src;
    str1 = strtok_r(pstr, ":", &str2);
    while (str1)
    {
        char *pend = NULL;
        if (num == -1)
        {
            num = (int)strtol((char *)str1, &pend, 10);
        }
        else
        {
            *(pfloat + i) = (float)strtof((char *)str1, &pend);
            ++i;
        }
        //*pfloat = strtof((char *)src, &pend);

        if (((char *)src)[0] != '\0' && *pend == '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        //++pfloat;
        if (i >= (*dest_size) / (int)sizeof(float))
        {
            ERRLOG("buffer not enough[%d/%d]", src_len, *dest_size);
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    if (num >= 0 && num == i)
    {
        *dest_size = num * (int)sizeof(float);
        return RET_RIGHT;
    }
    else
    {
        ERRLOG("format error!");
        return RET_WRONG;
    }
}

ERR_CODE string_array_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    CHECK_PARAM((!dest || !src || *dest_size < (int)sizeof(char *) || src_len <= 0),
                "param error!");

    char **pstring = (char **)dest;
    int num = -1;
    int i = 0;
    char *str1 = NULL;
    char *str2 = NULL;
    char *pstr = (char *)src;
    str1 = strtok_r(pstr, ":", &str2);
    while (str1)
    {
        if (num == -1)
        {
            char *pend = NULL;
            num = (int)strtol(str1, &pend, 10);
            if (*pend != '\0')
            {
                ERRLOG("format error!");
                return RET_WRONG;
            }
        }
        else
        {
            if (strlen(str1) + 1 > MAX_STR_LEN)
            {
                ERRLOG("string too long[%d/%d]!", (int)strlen(str1) + 1, (int)MAX_STR_LEN);
                return LINE_TOO_LONG;
            }
            memcpy(*(pstring + i), str1, strlen(str1) + 1);
            ++i;
        }

        if (((char *)src)[0] != '\0')
        {
            //return RET_RIGHT;
        }
        else
        {
            ERRLOG("format error!");
            return RET_WRONG;
        }
        if (i >= (*dest_size) / (int)sizeof(char *))
        {
            ERRLOG("buffer not enough");
            return RET_WRONG;
        }

        str1 = strtok_r(NULL, ",", &str2);
    }

    if (num >= 0 && num == i)
    {
        *dest_size = num * (int)sizeof(char *);
        return RET_RIGHT;
    }
    else
    {
        ERRLOG("format error!");
        return RET_WRONG;
    }

}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
