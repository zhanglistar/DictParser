// comp_data_t handler implementaion

#include "comp_data_t_handler.h"

#define CHECK_PARAM(para, msg) if (para) { ERRLOG(msg); return PARAM_ERROR; }

ERR_CODE comp_data_t_handler(void *dest, 
                             int dest_len, 
                             const void *src, 
                             int src_len)
{
    CHECK_PARAM((!dest || !src || dest_len < sizeof(struct comp_data_t) || src_len <= 0), "param error!");

    struct comp_data_t *ptr = (struct comp_data_t *) dest;
    
#define CHECK_PTR_NULL(p) if(!p) { ERRLOG("format error!"); return RET_WRONG; }

    char *str1 = NULL;
    char *str2 = NULL;
    str1 = strtok_r((char *)src, ",", &str2);
    CHECK_PTR_NULL(str1);
    ptr->a = atoi(str1);
    str1 = strtok_r(NULL, ",", &str2);
    CHECK_PTR_NULL(str1);
    ptr->b = atof(str1);
    str1 = strtok_r(NULL, ",", &str2);
    CHECK_PTR_NULL(str1);
    if (strlen(str1) + 1 >= sizeof(ptr->c))
    {
        ERRLOG("string too long[%d/%d]!", strlen(str1)+1, sizeof(ptr->c));
        return RET_WRONG;
    }
    memcpy(ptr->c, str1, sizeof(ptr->c));

#undef CHECK_PTR_NULL

    return RET_RIGHT;
}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
