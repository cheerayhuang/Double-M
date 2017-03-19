/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: LogFile.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-16-2017 18:36:31
 * @version $Revision$
 *
 **************************************************************************/

#ifndef __DOUBLE_M_LOGFILE_H__
#define __DOUBLE_M_LOGFILE_H__

#include <stdio.h>
#include <string>

namespace mm {
// since every log message is not very large, using glibc FILE obj is more efficient.
class LogFile {

public:
    explicit LogFile(const std::string &file_name);

    LogFile(const LogFile &) = delete;
    LogFile(LogFile&&) = delete;
    LogFile& operator=(const LogFile&) = delete;
    LogFile& operator=(LogFile&&) = delete;

    ~LogFile();

    size_t Write(const char* data, size_t len) const;
    void Flush();

private:
    FILE* f_;
    char buffer_[64*1024]; // 64k
};
}

#endif

