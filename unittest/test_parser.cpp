
/***************************************************************************
 *
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
// unittest for Parser class.

#include <gtest/gtest.h>
#include "parser.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_parser: public ::testing::Test
{
public:
    test_parser() {}
    virtual ~test_parser() {}
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
    
public:
    Parser *_a_parser;
};

#define INIT(p) do { \
    _a_parser = new(std::nothrow) Parser(p); \
    ASSERT_TRUE(_a_parser); \
} while (0)

#define CLEAR() do { \
    delete _a_parser; \
    _a_parser = NULL; \
} while (0)

#define DICT_PATH "../dict/"

// empty name dict
TEST_F(test_parser, init_dict_name_empty)
{
    INIT("");
    EXPECT_EQ(RET_WRONG, _a_parser->init());
    CLEAR();
}

// a dict which doesn't exist
TEST_F(test_parser, init_dict_name_not_exist)
{
    INIT("./nofileexist");
    EXPECT_EQ(RET_WRONG, _a_parser->init());
    CLEAR();
}

// an empty dict 
TEST_F(test_parser, init_empty_dict)
{
    INIT(DICT_PATH "emptydict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int i;
    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(1, "int", &i, sizeof(i)));
    CLEAR();
}

// register null handler
TEST_F(test_parser, register_null_handler)
{
    INIT(DICT_PATH "emptydict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_WRONG, _a_parser->register_handler("TESTTYPE", NULL));
    CLEAR();
}

// a faked handler
ERR_CODE test_int_handler(void *dest, int , const void *src, int )
{
    return RET_RIGHT;
}
// register a handler
TEST_F(test_parser, register_real_handler)
{
    INIT(DICT_PATH "emptydict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_RIGHT, _a_parser->register_handler("int", test_int_handler));
    CLEAR();
}

// only one line dict containing builtin type int, float, double, char *
TEST_F(test_parser, parse_data_one_line)
{
    INIT(DICT_PATH "int_float_double_string.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a;
    float b;
    double c;
    char d[1024];
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(4,
                "int", &a, sizeof(a),
                "float", &b, sizeof(b),
                "double", &c, sizeof(c),
                "string", &d, sizeof(d)));
    ASSERT_TRUE(a==1);
    ASSERT_TRUE(b-2.3 < 0.0001);
    ASSERT_TRUE(c-4.567 < 0.0001);
    ASSERT_TRUE(strcmp(d, "this is a test") == 0);
    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(4,
                "int", &a, sizeof(a),
                "float", &b, sizeof(b),
                "double", &c, sizeof(c),
                "string", &d, sizeof(d)));
    CLEAR();
}

// a dict containing an empty line dict
TEST_F(test_parser, parse_data_empty_line)
{
    INIT(DICT_PATH "first_line_empty.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a;
    double c;
    EXPECT_EQ(RET_WRONG, _a_parser->parse_data_line_by_line(2,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c)));
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(2,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c)));
    ASSERT_TRUE(a==1);
    ASSERT_TRUE(c-0.123456 < 0.0001);

    CLEAR();
}

// a dict containing too long line and error-format line
TEST_F(test_parser, parse_data_too_long_line_format_error)
{
    INIT(DICT_PATH "second_line_toolong.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a;
    double c;
    char astring[1024];
    EXPECT_EQ(RET_RIGHT, _a_parser->parse_data_line_by_line(3,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c),
                "string", &astring, sizeof(astring)));
    ASSERT_TRUE(a==1);
    ASSERT_TRUE(c-5.0 < 0.001);
    ASSERT_TRUE(strcmp(astring, "thisisanewtest") == 0);

    EXPECT_EQ(LINE_TOO_LONG, _a_parser->parse_data_line_by_line(3,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c),
                "string", &astring, sizeof(astring)));

    EXPECT_EQ(RET_WRONG, _a_parser->parse_data_line_by_line(3,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c),
                "string", &astring, sizeof(astring)));

    EXPECT_EQ(FILE_END, _a_parser->parse_data_line_by_line(3,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c),
                "string", &astring, sizeof(astring)));
    CLEAR();
}

// read all data from dict
TEST_F(test_parser, parse_data_read_all_data)
{
    INIT(DICT_PATH "int_array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a;
    double c;
    char astring[1024];
    ERR_CODE ret = _a_parser->parse_data_line_by_line(3,
                "int", &a, sizeof(a),
                "double", &c, sizeof(c),
                "string", &astring, sizeof(astring));
    ASSERT_TRUE(RET_RIGHT == ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(a==1);
        ASSERT_TRUE(c-5.0 < 0.001);
        ASSERT_TRUE(strcmp(astring, "thisisanewtest") == 0);

        ret = _a_parser->parse_data_line_by_line(3,
            "int", &a, sizeof(a),
            "double", &c, sizeof(c),
            "string", &astring, sizeof(astring));
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

// a dict each colomn is array
TEST_F(test_parser, parse_data_array)
{
    INIT(DICT_PATH "array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a[1024];
    float b[1024];
    double c[1024];
    ERR_CODE ret;
    ret = _a_parser->set_format("intarray\tfloatarray\tdoublearray");
    ASSERT_TRUE(ret == RET_RIGHT);
    ret = _a_parser->parse_data_line_by_line(3,
                a, sizeof(a),
                b, sizeof(b),
                c, sizeof(c));
    EXPECT_EQ(RET_RIGHT, ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(a[0]==1);
        ASSERT_TRUE(a[1]==2);
        ASSERT_TRUE(a[2]==3);
        ASSERT_TRUE(a[3]==4);
        ASSERT_TRUE(b[0]-1.0 < 0.001);
        ASSERT_TRUE(b[1]-2.0 < 0.001);
        ASSERT_TRUE(b[2]-3.0 < 0.001);
        ASSERT_TRUE(b[3]-4.0 < 0.001);
        ASSERT_TRUE(c[0]-5.0 < 0.001);
        ASSERT_TRUE(c[1]-6.0 < 0.001);
        ASSERT_TRUE(c[2]-7.0 < 0.001);
        ASSERT_TRUE(c[3]-8.0 < 0.001);

        ret = _a_parser->parse_data_line_by_line(3,
                    a, sizeof(a),
                    b, sizeof(b),
                    c, sizeof(c));
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

// user define type
struct mytype_t
{
    int aint;
    string astring;
    double adouble;
};
// user define handler
ERR_CODE mytype_handler(void *dest, int dlen, const void *src, int slen)
{
    if (!dest || !src || dlen <=0 || slen <= 0)
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
TEST_F(test_parser, parse_data_user_define_handler)
{
    INIT(DICT_PATH "user_define_handler.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    EXPECT_EQ(RET_RIGHT, _a_parser->register_handler("mytype_t", mytype_handler));
    int ret;
    EXPECT_EQ(RET_RIGHT, _a_parser->set_format("intarray\tmytype_t"));

    int aint[1024];
    mytype_t mt;
    ret = _a_parser->parse_data_line_by_line(2,
                aint, sizeof(aint),
                &mt, sizeof(mt));
    EXPECT_EQ(RET_RIGHT, ret);
    while (ret == RET_RIGHT)
    {
        ASSERT_TRUE(aint[0]==1);
        ASSERT_TRUE(aint[1]==2);
        ASSERT_TRUE(aint[2]==3);
        ASSERT_TRUE(aint[3]==4);
        ASSERT_TRUE(mt.aint == 1024);
        ASSERT_TRUE(strcmp(mt.astring.c_str(), "test") == 0);
        ASSERT_TRUE(mt.adouble - 8.0 < 0.001);

        ret = _a_parser->parse_data_line_by_line(2,
                    aint, sizeof(aint),
                    &mt, sizeof(mt));
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

// test load_all
TEST_F(test_parser, parse_data_load_all)
{
    INIT(DICT_PATH "loadall_array.dict");
    EXPECT_EQ(RET_RIGHT, _a_parser->init());
    int a[1024];
    float b[1024];
    double c[1024];
    ERR_CODE ret;
    ret = _a_parser->set_format("intarray\tfloatarray\tdoublearray");
    ASSERT_TRUE(ret == RET_RIGHT);
    EXPECT_EQ(2, _a_parser->load_all());
    int i = 1;
    ret = _a_parser->parse_data_by_line(i, 3, a, sizeof(a), b, sizeof(b), c, sizeof(c));
    while (ret != FILE_END)
    {
        ASSERT_TRUE(a[0]==1);
        ASSERT_TRUE(a[1]==2);
        ASSERT_TRUE(a[2]==3);
        ASSERT_TRUE(a[3]==4);
        ASSERT_TRUE(b[0]-1.0 < 0.001);
        ASSERT_TRUE(b[1]-2.0 < 0.001);
        ASSERT_TRUE(b[2]-3.0 < 0.001);
        ASSERT_TRUE(b[3]-4.0 < 0.001);
        ASSERT_TRUE(c[0]-5.0 < 0.001);
        ASSERT_TRUE(c[1]-6.0 < 0.001);
        ASSERT_TRUE(c[2]-7.0 < 0.001);
        ASSERT_TRUE(c[3]-8.0 < 0.001);
        ++i;
        ret = _a_parser->parse_data_by_line(i, 3, a, sizeof(a), b, sizeof(b), c, sizeof(c));
    }
    ASSERT_TRUE(ret == FILE_END);

    CLEAR();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
