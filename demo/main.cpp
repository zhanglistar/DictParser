/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file demo/main.cpp
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

//#include <iostream>
#include "dict_parser.h"
#include "comp_data_t_handler.h"

int main(int argc, char *argv[])
{
    // demo: deal with builtin type
    const char *filename = "./int_float_string.dict";

    DictParser obj(filename);

    int ret = obj.init();
    if (ret)
    {
        ERRLOG("init DictParser object failed!");
        return -1;
    }

    ret = obj.set_format("int,float,string", ",");
    if (ret)
    {
        ERRLOG("set_format failed!");
        return -1;
    }

    printf("================Begin reading dict:================\n");
    printf("file name :%s\n", filename);
    int i = 0;
    float f = 0;
    char str[MAX_STR_LEN];
    int isize = sizeof(i);
    int fsize = sizeof(f);
    int strsize = sizeof(str);
    ret = obj.parse_data_line_by_line(3, &i, &isize, &f, &fsize, str, &strsize);
    while (ret != FILE_END)
    {
        if (ret == RET_RIGHT)
        {
            //std::cout << i << "\t" << f << "\t" << str << std::endl;
            printf("%d\t%f\t%s\n", i, f, str);
        }
        isize = sizeof(i);
        fsize = sizeof(f);
        strsize = sizeof(str);
        ret = obj.parse_data_line_by_line(3, &i, &isize,
                    &f, &fsize, str, &strsize);
    }
    printf("=======================End=========================\n");

    // demo: deal with complecated data
    const char *filename1 = "./comp.dict";
    DictParser obj1(filename1);
    ret = obj1.init();
    if (ret)
    {
        ERRLOG("init DictParser object failed!");
        return -1;
    }

    ret = obj1.register_handler("my_data_type", comp_data_t_handler);
    if (ret)
    {
        ERRLOG("register_handler failed!");
        return -1;
    }

    printf("================Begin reading dict:================\n");
    comp_data_t tmp_compdata;
    isize = sizeof(i);
    fsize = sizeof(f);
    strsize = sizeof(str);
    int comp_data_size = sizeof(tmp_compdata);
    ret = obj1.parse_data_line_by_line(4, "string", str, &strsize,
            "int", &i, &isize,
            "float", &f, &fsize,
            "my_data_type", &tmp_compdata, &comp_data_size);
    while (ret != FILE_END)
    {
        if (ret == RET_RIGHT)
        {
            printf("%s\t%d\t%f\t%d,%f,%s\n", str, i, f, tmp_compdata.a,
                tmp_compdata.b, tmp_compdata.c);
        }

        isize = sizeof(i);
        fsize = sizeof(f);
        strsize = sizeof(str);
        comp_data_size = sizeof(tmp_compdata);
        ret = obj1.parse_data_line_by_line(4, "string", str, &strsize,
                "int", &i, &isize,
                "float", &f, &fsize,
                "my_data_type", &tmp_compdata, &comp_data_size);
    }
    printf("=======================End=========================\n");

    // demo: deal with builtin array type
    const char *filename2 = "./int_float_string_array.dict";

    DictParser obj2(filename2);

    ret = obj2.init();
    if (ret)
    {
        ERRLOG("init DictParser object failed!");
        return -1;
    }

    ret = obj2.set_format("intarray,floatarray,stringarray", ",");
    if (ret)
    {
        ERRLOG("set_format failed!");
        return -1;
    }

    printf("================Begin reading dict:================\n");
    printf("file name :%s\n", filename2);
    int int_arr[MAX_ARR_SIZE];
    int int_arr_size = sizeof(int_arr);
    float float_arr[MAX_ARR_SIZE];
    int float_arr_size = sizeof(float_arr);
    char *string_arr[MAX_ARR_SIZE];
    for (int i = 0; i < MAX_ARR_SIZE; ++i)
    {
        string_arr[i] = new char[MAX_STR_LEN];
    }
    int string_arr_size = sizeof(string_arr);
    ret = obj2.parse_data_line_by_line(3, int_arr, &int_arr_size, float_arr,
        &float_arr_size, string_arr, &string_arr_size);
    while (ret != FILE_END)
    {
        if (ret == RET_RIGHT)
        {
            int int_arr_len = int_arr_size / (int)sizeof(int);
            printf("%d:", int_arr_len);
            for (int i = 0; i < int_arr_len; ++i)
            {
                printf("%d", int_arr[i]);
                if (i != int_arr_len - 1)
                {
                    printf(",");
                }
            }
            printf("\t");
            int float_arr_len = float_arr_size / (int)sizeof(float);
            printf("%d:", float_arr_len);
            for (int i = 0; i < float_arr_len; ++i)
            {
                printf("%f", float_arr[i]);
                if (i != float_arr_len - 1)
                {
                    printf(",");
                }
            }
            printf("\t");
            int string_arr_len = string_arr_size / (int)sizeof(char *);
            printf("%d:", string_arr_len);
            for (int i = 0; i < string_arr_len; ++i)
            {
                printf("%s", string_arr[i]);
                if (i != string_arr_len - 1)
                {
                    printf(",");
                }
            }
            printf("\n");
        }
        int_arr_size = sizeof(int_arr);
        float_arr_size = sizeof(float_arr);
        string_arr_size = sizeof(string_arr);
        ret = obj2.parse_data_line_by_line(3, int_arr, &int_arr_size,
            float_arr, &float_arr_size, string_arr, &string_arr_size);
    }
    for (int i = 0; i < MAX_ARR_SIZE; ++i)
    {
        SAFE_DELETE_ARRAY(string_arr[i]);
    }
    printf("=======================End=========================\n");

    // demo: load all dict
    const char *filename3 = "./loadall_array.dict";

    DictParser obj3(filename3);
    ret = obj3.init();
    if (ret)
    {
        ERRLOG("init DictParser object failed!");
        return -1;
    }

    ret = obj3.register_handler("my_data_type", comp_data_t_handler);
    if (ret)
    {
        ERRLOG("register_handler failed!");
        return -1;
    }

    ret = obj3.set_format("string\tfloatarray\tmy_data_type");
    if (ret)
    {
        ERRLOG("set_format failed!");
        return -1;
    }

    int linenum = obj3.load_all();
    if (linenum == RET_WRONG)
    {
        ERRLOG("load_all() failed!");
        return -1;
    }

    printf("================Begin reading dict:================\n");
    printf("file name :%s\n", filename3);
    for (int l = 1; l <= linenum; ++l)
    {
        strsize = sizeof(str);
        float_arr_size = sizeof(float_arr);
        comp_data_size = sizeof(tmp_compdata);
        ret = obj3.parse_data_by_line(l, 3, str, &strsize,
                    float_arr, &float_arr_size, &tmp_compdata, &comp_data_size);
        if (ret == RET_RIGHT)
        {
            printf("%s\t", str);
            int float_arr_len = float_arr_size / (int)sizeof(float);
            printf("%d:", float_arr_len);
            for (int i = 0; i < float_arr_len; ++i)
            {
                printf("%f", float_arr[i]);
                if (i != float_arr_len - 1)
                {
                    printf(",");
                }
            }
            printf("\t");
            printf("%d,%f,%s\n", tmp_compdata.a, tmp_compdata.b, tmp_compdata.c);
        }
    }
    printf("=======================End=========================\n");

    return 0;
}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
