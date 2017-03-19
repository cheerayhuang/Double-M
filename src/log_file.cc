/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: LogFile.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-16-2017 20:08:53
 * @version $Revision$
 *
 **************************************************************************/

#include "log_file.h"

#include <assert.h>
#include <stdio.h>

using namespace mm;

LogFile::LogFile(const std::string &file_name)
    : f_(::fopen(file_name.c_str(), "a"))
{
    assert(f_);
    ::setbuffer(f_, buffer_, sizeof buffer_);
}

size_t LogFile::Write(const char* data, size_t len) const {

    return ::fwrite_unlocked(data, 1, len, f_);
}

void LogFile::Flush() {
    ::fflush(f_);
}

LogFile::~LogFile() {
    fclose(f_);
}
