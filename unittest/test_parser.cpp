/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file TestParser.h
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

#include <gtest/gtest.h>
#include "dict_parser.h"
#include <string>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class TestParser : public ::testing::Test
{
public:
    TestParser() {}
    virtual ~TestParser() {}
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }

public:
    DictParser *_a_parser;
};

#define INIT(p) do { \
    _a_parser = new(std::nothrow) DictParser(p); \
    ASSERT_TRUE(_a_parser); \
} while (0)

#define CLEAR() do { \
    delete _a_parser; \
    _a_parser = NULL; \
} while (0)

// empty name dict
TEST_F(TestParser, init_dict_name_empty)
{
    INIT("");
    EXPECT_EQ(RET_WRONG, _a_parser->init());
    CLEAR();
}

// a dict which doesn't exist
TEST_F(TestParser, init_dict_name_not_exist)
{
    INIT("./nofileexist");
    EXPECT_EQ(RET_WRONG, _a_parser->init());
    CLEAR();
}

// an empty dict
TEST_F(TestParser, init_empty_dict)
{
    INIT("../dict/empty.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    int isize = sizeof(i);
    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(1, "int", &i, &isize));
    CLEAR();
}

// register null handler
TEST_F(TestParser, register_null_handler)
{
    INIT("../dict/empty.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_WRONG, _a_parser->register_handler("TESTTYPE", NULL));
    CLEAR();
}

// a faked handler
ERR_CODE test_int_handler(void *dest, int *dest_size, const void *src, int src_len)
{
    return RET_RIGHT;
}
// register a handler
TEST_F(TestParser, register_real_handler)
{
    INIT("../dict/empty.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_RIGHT, _a_parser->register_handler("int", test_int_handler));
    CLEAR();
}

// only one line dict containing builtin type int, float, double, char *
TEST_F(TestParser, parse_data_one_line)
{
    INIT("../dict/int_float_string.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    float f;
    char str[1024];
    int isize = sizeof(i);
    int fsize = sizeof(f);
    int strsize = sizeof(str);
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));
    ASSERT_TRUE(i == 1);
    ASSERT_TRUE(f-2.3 < 0.0001);
    ASSERT_TRUE(strcmp(str, "this is a test") == 0);
    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));
    CLEAR();
}

// a dict containing an empty line dict
TEST_F(TestParser, parse_data_empty_line)
{
    INIT("../dict/first_line_empty.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    float f;
    int isize = sizeof(i);
    int fsize = sizeof(f);
    EXPECT_EQ(RET_WRONG, _a_parser->parse_data_line_by_line(2,
                "int", &i, &isize,
                "float", &f, &fsize));
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(2,
                "int", &i, &isize,
                "float", &f, &fsize));
    ASSERT_TRUE(i == 1);
    ASSERT_TRUE(f-0.123456 < 0.0001);

    CLEAR();
}

// a dict containing too long line and error-format line
TEST_F(TestParser, parse_data_too_long_line_format_error)
{
    INIT("../dict/second_line_toolong.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    float f;
    char str[1024];
    int isize = sizeof(i);
    int fsize = sizeof(f);
    int strsize = sizeof(str);
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));
    ASSERT_TRUE(i == 1);
    ASSERT_TRUE(f-5.0 < 0.001);
    ASSERT_TRUE(strcmp(str, "thisisanewtest") == 0);

    EXPECT_EQ(LINE_TOO_LONG, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));

    EXPECT_EQ(RET_WRONG, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));

    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize));
    CLEAR();
}

// read all data from dict
TEST_F(TestParser, parse_data_read_all_data)
{
    INIT("../dict/int_array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    float f;
    char str[1024];
    int isize = sizeof(i);
    int fsize = sizeof(f);
    int strsize = sizeof(str);
    ERR_CODE ret = _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize);
    ASSERT_TRUE(RET_RIGHT == ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(i == 1);
        ASSERT_TRUE(f-5.0 < 0.001);
        ASSERT_TRUE(strcmp(str, "thisisanewtest") == 0);

        ret = _a_parser->parse_data_line_by_line(3,
                "int", &i, &isize,
                "float", &f, &fsize,
                "string", str, &strsize);
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

// a dict each colomn is array
TEST_F(TestParser, parse_data_array)
{
    INIT("../dict/array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int int_arr[1024];
    float float_arr[1024];
    char *string_arr[1024];
    for (int i = 0; i < 1024; ++i)
    {
        string_arr[i] = new char[1024];
    }
    int int_arr_size = sizeof(int_arr);
    int float_arr_size = sizeof(float_arr);
    int string_arr_size = sizeof(string_arr);
    ERR_CODE ret;
    ret = _a_parser->set_format("intarray\tfloatarray\tstringarray");
    ASSERT_TRUE(ret == RET_RIGHT);
    ret = _a_parser->parse_data_line_by_line(3,
                int_arr, &int_arr_size,
                float_arr, &float_arr_size,
                string_arr, &string_arr_size);
    EXPECT_EQ(RET_RIGHT, ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(int_arr_size == 4*sizeof(int));
        ASSERT_TRUE(int_arr[0] == 1);
        ASSERT_TRUE(int_arr[1] == 2);
        ASSERT_TRUE(int_arr[2] == 3);
        ASSERT_TRUE(int_arr[3] == 4);
        ASSERT_TRUE(float_arr_size == 4*sizeof(float));
        ASSERT_TRUE(float_arr[0]-1.0 < 0.001);
        ASSERT_TRUE(float_arr[1]-2.0 < 0.001);
        ASSERT_TRUE(float_arr[2]-3.0 < 0.001);
        ASSERT_TRUE(float_arr[3]-4.0 < 0.001);
        ASSERT_TRUE(string_arr_size == 4*sizeof(char *));
        ASSERT_TRUE(strcmp(string_arr[0], "5.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[1], "6.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[2], "7.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[3], "8.0") == 0);

        int_arr_size = sizeof(int_arr);
        float_arr_size = sizeof(float_arr);
        string_arr_size = sizeof(string_arr);
        ret = _a_parser->parse_data_line_by_line(3,
                    int_arr, &int_arr_size,
                    float_arr, &float_arr_size,
                    string_arr, &string_arr_size);
    }
    ASSERT_TRUE(ret == FILE_END);
    for (int i = 0; i < 1024; ++i)
    {
        SAFE_DELETE_ARRAY(string_arr[i]);
    }
    CLEAR();
}

// user define type
struct mytype_t
{
    int aint;
    std::string astring;
    double adouble;
};
// user define handler
ERR_CODE mytype_handler(void *dest, int *dest_size, const void *src, int slen)
{
    if (!dest || !src || *dest_size < (int)sizeof(mytype_t) || slen <= 0)
    {
        fprintf(stderr, "Param error!\n");
        return RET_WRONG;
    }

    mytype_t *pdata = (mytype_t *) dest;
    char *pstr = (char *) src;
    char *p1 = NULL;
    char *p2 = NULL;
    char *pend = NULL;
    p1 = strtok_r(pstr, ",", &p2);
    if (p1)
    {
        pdata->aint = (int)strtol(p1, &pend, 10);
        if (p1[0] != '\0' && *pend == '\0')
        {
        }
        else
        {
            fprintf(stderr, "Format error!\n");
            return RET_WRONG;
        }
    }
    else
    {
        fprintf(stderr, "Format error!\n");
        return RET_WRONG;
    }

    p1 = strtok_r(NULL, ",", &p2);
    if (p1)
    {
        pdata->astring = p1;
    }
    else
    {
        fprintf(stderr, "Format error!\n");
        return RET_WRONG;
    }

    p1 = strtok_r(NULL, ",", &p2);
    if (p1)
    {
        pdata->adouble = strtod(p1, &pend);
        if (p1[0] != '\0' && *pend == '\0')
        {
        }
        else
        {
            fprintf(stderr, "Format error!\n");
            return RET_WRONG;
        }
    }
    else
    {
        fprintf(stderr, "Format error!\n");
        return RET_WRONG;
    }

    return RET_RIGHT;
}
// test user define handler
TEST_F(TestParser, parse_data_user_define_handler)
{
    INIT("../dict/user_define_handler.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_RIGHT, _a_parser->register_handler("mytype_t", mytype_handler));
    int ret;
    EXPECT_EQ(RET_RIGHT, _a_parser->set_format("intarray\tmytype_t"));

    int int_arr[1024];
    mytype_t mt;
    int int_arr_size = sizeof(int_arr);
    int mttype_size = sizeof(mt);
    ret = _a_parser->parse_data_line_by_line(2,
                int_arr, &int_arr_size,
                &mt, &mttype_size);
    EXPECT_EQ(RET_RIGHT, ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(int_arr_size == 4*sizeof(int));
        ASSERT_TRUE(int_arr[0] == 1);
        ASSERT_TRUE(int_arr[1] == 2);
        ASSERT_TRUE(int_arr[2] == 3);
        ASSERT_TRUE(int_arr[3] == 4);
        ASSERT_TRUE(mt.aint == 1024);
        ASSERT_TRUE(strcmp(mt.astring.c_str(), "test") == 0);
        ASSERT_TRUE(mt.adouble - 8.0 < 0.001);

        int_arr_size = sizeof(int_arr);
        mttype_size = sizeof(mt);
        ret = _a_parser->parse_data_line_by_line(2,
                    int_arr, &int_arr_size,
                    &mt, &mttype_size);
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

// test load_all
TEST_F(TestParser, parse_data_load_all)
{
    INIT("../dict/loadall_array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int int_arr[1024];
    float float_arr[1024];
    char *string_arr[1024];
    for (int i = 0; i < 1024; ++i)
    {
        string_arr[i] = new char[1024];
    }
    int int_arr_size = sizeof(int_arr);
    int float_arr_size = sizeof(float_arr);
    int string_arr_size = sizeof(string_arr);
    ERR_CODE ret;
    ret = _a_parser->set_format("intarray\tfloatarray\tstringarray");
    ASSERT_TRUE(ret == RET_RIGHT);
    EXPECT_EQ(5, _a_parser->load_all());
    int i = 1;
    ret = _a_parser->parse_data_by_line(i, 3,
                int_arr, &int_arr_size,
                float_arr, &float_arr_size,
                string_arr, &string_arr_size);
    while (ret != FILE_END)
    {
        ASSERT_TRUE(int_arr_size == 4*sizeof(int));
        ASSERT_TRUE(int_arr[0] == 1);
        ASSERT_TRUE(int_arr[1] == 2);
        ASSERT_TRUE(int_arr[2] == 3);
        ASSERT_TRUE(int_arr[3] == 4);
        ASSERT_TRUE(float_arr_size == 4*sizeof(float));
        ASSERT_TRUE(float_arr[0]-1.0 < 0.001);
        ASSERT_TRUE(float_arr[1]-2.0 < 0.001);
        ASSERT_TRUE(float_arr[2]-3.0 < 0.001);
        ASSERT_TRUE(float_arr[3]-4.0 < 0.001);
        ASSERT_TRUE(string_arr_size == 4*sizeof(char *));
        ASSERT_TRUE(strcmp(string_arr[0], "5.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[1], "6.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[2], "7.0") == 0);
        ASSERT_TRUE(strcmp(string_arr[3], "8.0") == 0);

        int_arr_size = sizeof(int_arr);
        float_arr_size = sizeof(float_arr);
        string_arr_size = sizeof(string_arr);
        ++i;
        ret = _a_parser->parse_data_by_line(i, 3,
                    int_arr, &int_arr_size,
                    float_arr, &float_arr_size,
                    string_arr, &string_arr_size);
    }
    ASSERT_TRUE(ret == FILE_END);
    for (int i = 0; i < 1024; ++i)
    {
        SAFE_DELETE_ARRAY(string_arr[i]);
    }

    CLEAR();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
