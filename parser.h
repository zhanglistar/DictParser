/*
 * =====================================================================================
 *
 *       Filename:  parser.h
 *
 *    Description:  data parser class, NOT thread-safe.
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

#ifndef __CODE_MASTER_PARSER_H__
#define __CODE_MASTER_PARSER_H__

#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#define ERRLOG(fmt, arg...) do { \
    printf("[%s() %s:%d]" fmt "\n", __FUNCTION__, __FILE__, __LINE__, ## arg);\
} while(0)

#define ERRLOG_NL(fmt, arg...) do { \
    printf("[%s() %s:%d]" fmt, __FUNCTION__, __FILE__, __LINE__, ## arg);\
} while(0)

#define SAFE_FREE(p) { if(p) { free(p); p = NULL; } }

// return code
enum ERR_CODE
{
    LINE_TOO_LONG = -100,
    PARAM_ERROR,
    RET_WRONG = -1,
    RET_RIGHT = 0,
    FILE_END
};

// type handler
typedef ERR_CODE (* parse_func_t)(void *dest, int , const void *src, int );

struct type_handler_t
{
    char *name;
    parse_func_t handler;
};

class Parser
{
public:
    /**
     * @brief constructor
     *
     * @param fn): dict name
     */
    explicit Parser(const std::string fn):
        _buffer(NULL),
        _fd(NULL),
        _linenum(0),
        _filename(fn), 
        _format(""),
        _seprator("\t")
    {
        _line_array.clear();
        _type_handler_map.clear();
        _type_array.clear();
        _err_map.clear();
    }

    /**
     * @brief constructor
     *
     * @param fn: dict name
     * @param sep): dict seperator each column
     */
    explicit Parser(const std::string fn, std::string sep):
        _buffer(NULL),
        _fd(NULL),
        _linenum(0),
        _filename(fn), 
        _format(""),
        _seprator(sep)
    {
        _line_array.clear();
        _type_handler_map.clear();
        _type_array.clear();
        _err_map.clear();
    }

    /**
     * @brief initialization 
     *
     * @return RET_RIGHT for success, other for fail
     */
    ERR_CODE init();

    /**
     * @brief register a new handler with a new type
     *
     * @param std::string: type name
     * @param parse_func_t : type handler
     *
     * @return RET_RIGHT for success, other for fail
     */
    ERR_CODE register_handler(std::string, parse_func_t);

    /**
     * @brief parse data. ATTENTION: use set_format first!
     *
     * @param cnt: num of variable parameters
     * @param ... : variable parameters, format is pointer of dest data, 
     *              following with size of dest data
     *
     * @return RET_RIGHT for success, other for fail
     */
    ERR_CODE parse_data_line_by_line(int cnt, ...);

    /**
     * @brief parse data. No need set_format first.
     *
     * @param cnt: num of variable parameters including first pair
     * @param type: type of column, eg. int
     * @param data_ptr: pointer of dest data buffer
     * @param sz: size of dest data buffer
     * @param ... : variable parameters, format is the same with the first
     *
     * @return RET_RIGHT for success, other for fail
     */
    ERR_CODE parse_data_line_by_line(int cnt, const char *type, void *data_ptr, int sz, ...);

    /**
     * @brief set dict format for column
     *
     * @param fmt: format string, eg, "int\tfloatarray\tmytype_t"
     * @param sep: seperator, default "\t". 
     *
     * @return RET_RIGHT for success, other for fail
     */
    ERR_CODE set_format(std::string fmt, const char *sep = "\t");

    /**
     * @brief load all lines into memory, but not parse.
     *
     * @return line number if success, RET_WRONG for fail.
     */
    int load_all();

    /**
     * @brief parse data by line number, which starts from 1
     *
     * @param linenum , line number, starts from 1
     * @param cnt, number of columns you want to read
     * @param ..., variable args.
     *
     * @return RET_RIGHT for success, FILE_END if ends, RET_WRONG for fail.
     */
    ERR_CODE parse_data_by_line(int linenum, int cnt, ...);

    /**
     * @brief parse data by line number, which starts from 1.
     *
     * Read as the format the user passes, ignore format setting in set_format().
     *
     * @param linenum, line number, starts from 1
     * @param cnt, number of columns you want to read
     * @param type, typename
     * @param data_ptr, dest data buffer pointer
     * @param sz, dest data buffer length
     * @param ..., variable args
     *
     * @return RET_RIGHT for success, FILE_END if ends, RET_WRONG for fail.
     */
    ERR_CODE parse_data_by_line(int linenum, 
                                int cnt, 
                                const char *type,
                                void *data_ptr, 
                                int sz, 
                                ...);
    /**
     * @brief deconstructor
     */
    ~Parser()
    {
        if (_fd)
        {
            fclose(_fd);
        }

        SAFE_FREE(_buffer);

        std::vector<char *>::const_iterator it = _line_array.begin();
        for (; it != _line_array.end() ; ++it)
        {
            free(*it);
        }
    }

private:
    /**
     * @brief register builtin handlers
     *
     * @return 0 for sucess, other for fail.
     */
    int _register_builtin_handlers();

    /**
     * @brief check if a type handler exists
     *
     * @param type, typename
     *
     * @return true if exists, false if not.
     */
    bool _check_handler_exists(const std::string &type);

    /**
     * @brief get a type handler
     *
     * @param type, typename
     *
     * @return function pointer of this type on success, NULL on fail.
     */
    parse_func_t _get_handler(const std::string &type);

    bool _is_line_valid(int line);

    ERR_CODE _get_line_error(int line);

    ERR_CODE _read_one_line(int &len);

    ERR_CODE _parse_data_internal3(int len, 
                                   int linenum, 
                                   int num, 
                                   const char *type, 
                                   void *data_ptr,
                                   int sz,
                                   va_list vl);
    ERR_CODE _parse_data_internal2(int len, int linenum, int num, va_list vl);

private:
    const static int _buf_size = 10240; // inner buffer size
    char *_buffer;                      // inner buffer
    FILE *_fd;                          // file descriptior of dict
    int _linenum;                       // current line number
    std::string _filename;              // dict name
    std::string _format;                // dict format
    std::string _seprator;              // dict seperator
    std::map<std::string, parse_func_t> _type_handler_map; // type-handler map
    std::vector<std::string> _type_array;                  // all registered types
    std::vector<char *> _line_array;    // pointer of each line in _all_content_buffer
    std::map<int, int> _err_map; // line - error map
};

#endif

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
