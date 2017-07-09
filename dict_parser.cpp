/***************************************************************************
 *
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file dict_parser.cpp
 * @author chenming05(com@baidu.com)
 * @date 2017/06/21 14:33:48
 * @brief
 *
 **/

#include "dict_parser.h"
#include <stdarg.h>
#include <errno.h>
#include "builtin_handler.h"

type_handler_t g_builtin_type_handler_array[] = {
    // builtin type
    { "int", int_handler },
    { "float", float_handler },
    { "string", string_handler },
    // builtin type array
    { "intarray", int_array_handler },
    { "floatarray", float_array_handler },
    { "stringarray", string_array_handler },
    { NULL, NULL },
};

const std::string g_null_str("");

int trim(char *buffer, int &len);

ERR_CODE DictParser::parse_data_line_by_line(int cnt, ...)
{
    // check parameters
    if (cnt > (int)_type_array.size())
    {
        ERRLOG("count too big, max[%u]", (int)_type_array.size());
        return RET_WRONG;
    }
    // read one line
    int len = 0;
    ERR_CODE ret = _read_one_line(len);
    if (ret != RET_RIGHT)
    {
        return ret;
    }
    // parser data
    va_list vl;
    va_start(vl, cnt);
    ret = _parse_data_internal2(len, _linenum, cnt, vl);
    va_end(vl);

    return ret;
}

ERR_CODE DictParser::parse_data_line_by_line(int cnt,
                                        const char *type,
                                        void *data_ptr,
                                        int *sz,
                                        ...)
{
    // check parameters
    if (!type || !data_ptr || *sz <= 0)
    {
        return PARAM_ERROR;
    }
    // read one line
    int len = 0;
    ERR_CODE ret = _read_one_line(len);
    if (ret != RET_RIGHT)
    {
        return ret;
    }
    // parser data
    va_list vl;
    va_start(vl, sz);
    ret = _parse_data_internal3(len, _linenum, cnt, type, data_ptr, sz, vl);
    va_end(vl);

    return ret;
}

inline bool DictParser::_check_handler_exists(const std::string &type)
{
    return (_type_handler_map.find(type) != _type_handler_map.end());
}

ERR_CODE DictParser::set_format(std::string fmt, const char *sep)
{
    if (fmt.length() <= 0 ||
        sep == NULL)
    {
        return RET_WRONG;
    }

    char *str1 = NULL;
    char *str2 = NULL;
    _format = fmt;
    int buf_size = fmt.length();
    char *buffer = new char[buf_size+1];
    if (buffer == NULL)
    {
        return RET_WRONG;
    }

    snprintf(buffer, buf_size+1, "%s", fmt.c_str());

    str1 = strtok_r(buffer, sep, &str2);
    while (str1)
    {
        if (!_check_handler_exists(str1))
        {
            ERRLOG("type[%s] handler not register", str1);
            SAFE_DELETE_ARRAY(buffer);
            return RET_WRONG;
        }
        _type_array.push_back(str1);
        str1 = strtok_r(NULL, sep, &str2);
    }

    SAFE_DELETE_ARRAY(buffer);
    return RET_RIGHT;
}

ERR_CODE DictParser::register_handler(std::string type_name, parse_func_t handler)
{
    if (type_name.length() <= 0 ||
        handler == NULL)
    {
        return RET_WRONG;
    }

    _type_handler_map[type_name] = handler;

    return RET_RIGHT;
}

ERR_CODE DictParser::init()
{
    if (_filename.length() <= 0)
    {
        ERRLOG("filename empty!");
        return RET_WRONG;
    }
    _fd = fopen(_filename.c_str(), "r");
    if (_fd == NULL)
    {
        ERRLOG("open %s failed!", _filename.c_str());
        return RET_WRONG;
    }

    _buffer = new char[_buf_size];
    if (!_buffer)
    {
        ERRLOG("no memory!");
        return RET_WRONG;
    }

    if (_register_builtin_handlers() != RET_RIGHT)
    {
        ERRLOG("_register_builtin_handlers failed!");
        return RET_WRONG;
    }

    return RET_RIGHT;
}

int DictParser::_register_builtin_handlers()
{
    int ret = RET_RIGHT;
    type_handler_t *p = g_builtin_type_handler_array;
    while (p->name && p->handler)
    {
        ret |= (int)register_handler(p->name, p->handler);
        ++p;
    }

    return ret;
}

inline parse_func_t DictParser::_get_handler(const std::string &type)
{
    if (_type_handler_map.find(type) == _type_handler_map.end())
    {
        return NULL;
    }

    return _type_handler_map[type];
}

int DictParser::load_all()
{
    // seek to beginning
    int ret = fseek(_fd, 0L, SEEK_SET);
    if (ret)
    {
        ERRLOG("fseek error[%s]", strerror(errno));
        return RET_WRONG;
    }
    // clear
    _linenum = 0;
    int len = 0;
    _line_array.clear();
    _line_array.push_back(NULL);
    // reading line by line
    while ((ret = _read_one_line(len)) != FILE_END)
    {
        _line_array.push_back(NULL);
        if (ret != RET_RIGHT)
        {
            _err_map[_linenum] = ret;
            continue;
        }
        // store line content
        char *line = new char[len + 2 + sizeof(int)];
        if (!line)
        {
            ERRLOG("no memory");
            _err_map[_linenum] = RET_WRONG;
            continue;
        }
        *(int *) line = len + 1;
        memcpy(line + sizeof(int), _buffer, len+1);
        _line_array[_linenum] = line;
    }

    return _linenum;
}

inline bool DictParser::_is_line_valid(int line)
{
    return _err_map.find(line) == _err_map.end();
}

inline ERR_CODE DictParser::_get_line_error(int line)
{
    return (ERR_CODE) _err_map[line];
}

ERR_CODE DictParser::parse_data_by_line(int linenum, int cnt, ...)
{
    if (linenum > _linenum)
    {
        ERRLOG("linenum too big[%d], total lines[%d]", linenum, _linenum);
        return FILE_END;
    }
    if (linenum <= 0)
    {
        ERRLOG("linenum starts from 1");
        return RET_WRONG;
    }
    if (!_is_line_valid(linenum))
    {
        return _get_line_error(linenum);
    }

    if (_line_array[linenum] == NULL)
    {
        ERRLOG("get line[%d] content failed!", linenum);
        return RET_WRONG;
    }
    int len = *(int *)_line_array[linenum];
    memcpy(_buffer, _line_array[linenum] + sizeof(int), len);

    va_list vl;
    va_start(vl, cnt);
    ERR_CODE ret = _parse_data_internal2(len, linenum, cnt, vl);
    va_end(vl);

    return ret;
}

ERR_CODE DictParser::parse_data_by_line(int linenum,
                                    int cnt,
                                    const char *type,
                                    void *data_ptr,
                                    int *sz,
                                    ...)
{
    if (!_is_line_valid(linenum))
    {
        return _get_line_error(linenum);
    }

    if (_line_array[linenum] == NULL)
    {
        ERRLOG("get line[%d] content failed!", linenum);
        return RET_WRONG;
    }
    int len = *(int *)_line_array[linenum];
    memcpy(_buffer, _line_array[linenum] + sizeof(int), len);

    va_list vl;
    va_start(vl, sz);
    ERR_CODE ret = _parse_data_internal3(len, linenum, cnt, type, data_ptr, sz, vl);
    va_end(vl);

    return ret;
}

ERR_CODE DictParser::_read_one_line(int &len)
{
    char *read_buf = fgets(_buffer, _buf_size, _fd);
    // error
    if (read_buf == NULL)
    {
        if (feof(_fd)) // file end
        {
            return FILE_END;
        }
        else
        {
            ERRLOG("fgets error[%s]!", strerror(errno));
            return RET_WRONG;
        }
    }
    ++_linenum;
    // empty line
    if (_buffer[0] == '\n' || _buffer[0] == '\r')
    {
        return RET_WRONG;
    }
    len = strlen(_buffer);
    // line too long
    if (_buffer[len-1] != '\n')
    {
        while (fgets(_buffer, _buf_size, _fd) != NULL)
        {
            if (_buffer[0] && _buffer[strlen(_buffer) - 1] == '\n')
            {
                break;
            }
        }
        if (feof(_fd))
        {
            return FILE_END;
        }
        return LINE_TOO_LONG;
    }
    // trim \n
    trim(_buffer, len);

    return RET_RIGHT;
}

ERR_CODE DictParser::_parse_data_internal3(int len,
                                       int linenum,
                                       int num,
                                       const char *type,
                                       void *data_ptr,
                                       int *sz,
                                       va_list vl)
{
    (void) len;
    const char *seprator = _seprator.c_str();
    char *str1 = NULL;
    char *str2 = NULL;
    str1 = strtok_r(_buffer, seprator, &str2);
    int i = 0;
    ERR_CODE ret;
    for (; i < num && str1; i++)
    {
        if (i == 0)
        {
            parse_func_t handle = _get_handler(type);
            if (!handle || (ret = (*handle)(data_ptr, sz, str1, strlen(str1)+1)) != RET_RIGHT)
            {
                ERRLOG("handle type[%s] failed, line[%d]", type, _linenum);
                return ret;
            }
        }
        else
        {
            const char *type_temp = va_arg(vl, char *);
            void *dest_ptr = va_arg(vl, void *);
            int *dest_len = va_arg(vl, int *);
            if (!type_temp || !dest_ptr)
            {
                ERRLOG("parametor illegal!");
                return RET_WRONG;
            }
            parse_func_t handle = _get_handler(type_temp);
            if (!handle || (ret = (*handle)(dest_ptr, dest_len, str1, strlen(str1)+1)) != RET_RIGHT)
            {
                ERRLOG("handle type[%s] failed, line[%d]", type_temp, _linenum);
                return ret;
            }
        }

        str1 = strtok_r(NULL, seprator, &str2);
    }

    if (i < num)
    {
        ERRLOG("format error!");
        return RET_WRONG;
    }

    return RET_RIGHT;
}

ERR_CODE DictParser::_parse_data_internal2(int len, int linenum, int num, va_list vl)
{
    (void) len;
    char *str1 = NULL;
    char *str2 = NULL;
    const char *seprator = _seprator.c_str();
    std::vector<std::string>::const_iterator it = _type_array.begin();
    str1 = strtok_r(_buffer, _seprator.c_str(), &str2);
    int i = 0;
    ERR_CODE ret;
    for (; i < num && str1; ++i, ++it)
    {
        void *data_ptr = va_arg(vl, void *);
        int *data_len = va_arg(vl, int *);
        parse_func_t handle = _get_handler(*it);

        if (!handle || (ret = (*handle)(data_ptr, data_len, str1, strlen(str1)+1)) != RET_RIGHT)
        {
            ERRLOG("parser type[%s] failed! Line[%d]", it->c_str(), linenum);
            return ret;
        }

        str1 = strtok_r(NULL, seprator, &str2);
    }

    if (i < num)
    {
        ERRLOG("line[%d] format error", linenum);
        return RET_WRONG;
    }

    return RET_RIGHT;
}

inline int trim(char *buffer, int &len)
{
    if (!buffer)
    {
        return RET_RIGHT;
    }

    while (buffer[len-1] == '\r' ||
           buffer[len-1] == '\n')
    {
        buffer[len-1] = '\0';
        --len;
    }

    return RET_RIGHT;
}

/*  vim: set ts=4 sw=4 sts=4 tw=100 */
