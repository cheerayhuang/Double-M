/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: async_log.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-16-2017 20:41:15
 * @version $Revision$
 *
 **************************************************************************/

#ifndef __DOUBLE_M_ASYNC_LOG_H__
#define __DOUBLE_M_ASYNC_LOG_H__

#include "mutex.h"
#include "thread.h"

#include <strings.h>
#include <string.h>

#include <vector>
#include <memory>
#include <string>


namespace mm {

class Buffer {
    enum {
       kLargeBufferLen = 1024 * 1024 // 1MB
    };

public:
    Buffer()
        : cur_(data_)
    {
        ::bzero(static_cast<void*>(data_), kLargeBufferLen);
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    ~Buffer() {}

    size_t Len() const { return cur_ - data_; }
    size_t AvailableLen() const { return kLargeBufferLen - Len(); }
    size_t Append(const char *data, size_t len) {
        assert(AvailableLen() >= len);
        ::memcpy(cur_, data, len);
        cur_ += len;
        return len;
    }

    /*void Clear() {
        cur_ = data_;
        ::bzero(data_, kLargeBufferLen);
    }*/

    bool Empty() { return cur_ == data_; }
    const char* cur() const { return cur_; }
    const char* data() const { return data_; }

private:
    char data_[kLargeBufferLen];
    char* cur_;
};

class AsyncLog {
public:
    AsyncLog(const std::string&);

    AsyncLog(const AsyncLog&) = delete;
    AsyncLog(AsyncLog&&) = delete;
    AsyncLog& operator=(const AsyncLog&) = delete;
    AsyncLog& operator=(AsyncLog&&) = delete;

    ~AsyncLog();

    void StartLogging();
    void StopLogging();
    void Append(const std::string&, size_t);
    void Append(const char*, size_t);

    size_t writen_nums() const { return writen_nums_; }

private:
    void ThreadFunc();

    MutexLock mutex_;
    MutexCond cond_;
    MutexCond cond_buffer_too_large_;
    Thread th_;

    std::unique_ptr<Buffer> buffer_;
    std::vector<std::unique_ptr<Buffer>> buffer_vec_;
    std::vector<std::unique_ptr<Buffer>> ready_buffer_vec_;


    std::string file_name_;
    bool running_;
    size_t writen_nums_;
};

}

#endif

