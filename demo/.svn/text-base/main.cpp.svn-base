/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  demo.
 *
 *        Version:  1.0
 *        Created:  2014年01月06日 15时06分06秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  zhangzhibiao01@
 *   Organization:  Baidu Inc.
 *
 * =====================================================================================
 */

#include <iostream>
#include "parser.h"
#include "comp_data_t_handler.h"

const int arr_siz = 1024;

int main(int argc, char *argv[])
{
    // demo: deal with builtin type
    const char *filename = "./intfloatstringdict";

    Parser obj(filename);

    int ret = obj.init();
    if (ret)
    {
        ERRLOG("init Parser object failed!");
        return -1;
    }

    ret = obj.set_format("int,float,string", ",");
    if (ret)
    {
        ERRLOG("set_format failed!");
        return -1;
    }

    std::cout << "Begin reading dict:\n================" << std::endl;
    int aint, bint, cint;
    float dfloat;
    char astring[arr_siz];
    ret = obj.parse_data_line_by_line(3, &aint, sizeof(aint), &dfloat, sizeof(dfloat), astring, sizeof(astring));
    while (ret != FILE_END)
    {
        if (ret == RET_RIGHT)
        {
            std::cout << aint << "\t" << dfloat << "\t" << astring << std::endl;
        }
        ret = obj.parse_data_line_by_line(3, &aint, sizeof(aint), 
                    &dfloat, sizeof(dfloat), astring, sizeof(astring));
    }
    std::cout << "================\nEnd" << std::endl;

    // demo: deal with complecated data
    const char *filename1 = "./compdict";
    Parser obj1(filename1);
    ret = obj1.init();
    if (ret)
    {
        ERRLOG("init Parser object failed!");
        return -1;
    }

    ret = obj1.register_handler("my_data_type", comp_data_t_handler);
    if (ret)
    {
        ERRLOG("register_handler failed!");
        return -1;
    }
    
    std::cout << "Begin reading dict:\n================" << std::endl;
    comp_data_t tmp_compdata;
    double dd;
    ret = obj1.parse_data_line_by_line(4, "string", astring, sizeof(astring),
            "int", &bint, sizeof(bint), 
            "double", &dd, sizeof(dd),
            "my_data_type", &tmp_compdata, sizeof(tmp_compdata));
    while (ret != FILE_END)
    {
        if (ret == RET_RIGHT)
        {
            std::cout << astring << "\t" << bint << "\t" << dd << "\t" <<
                    tmp_compdata.a << "," << 
                    tmp_compdata.b << "," << 
                    tmp_compdata.c << std::endl;
        }

        ret = obj1.parse_data_line_by_line(4, "string", astring, sizeof(astring),
            "int", &bint, sizeof(bint), 
            "double", &dd, sizeof(dd),
            "my_data_type", &tmp_compdata, sizeof(tmp_compdata));
    }
    std::cout << "================\nEnd" << std::endl;

    // demo: load all dict
    const char *filename2 = "./loadall_array.dict";

    Parser obj2(filename2);
    ret = obj2.init();
    if (ret)
    {
        ERRLOG("init Parser object failed!");
        return -1;
    }

    ret = obj2.set_format("intarray\tfloatarray\tdoublearray");
    if (ret)
    {
        ERRLOG("set_format failed!");
        return -1;
    }

    ret = obj2.load_all();
    if (ret == RET_WRONG)
    {
        ERRLOG("load_all() failed!");
        return -1;
    }

    std::cout << "Begin reading dict:\n================" << std::endl;
    int int_arr[arr_siz];
    float float_arr[arr_siz];
    double double_arr[arr_siz];
    for (int i = 1; i <= ret; i++)
    {
        ret = obj2.parse_data_by_line(i, 3, int_arr, sizeof(int_arr), 
                    float_arr, sizeof(float_arr), double_arr, sizeof(double_arr));
        if (ret == RET_RIGHT)
        {
            std::cout << int_arr[0] << "," << int_arr[1] << "," <<
                         int_arr[2] << "," << int_arr[3] <<
                         float_arr[0] << "," << float_arr[1] << "," <<
                         float_arr[2] << "," << float_arr[3] << "," <<
                         double_arr[0] << "," << double_arr[1] << "," <<
                         double_arr[2] << "," << double_arr[3] << "," << std::endl;

        }
    }
    std::cout << "================\nEnd" << std::endl;

    return 0;
}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
