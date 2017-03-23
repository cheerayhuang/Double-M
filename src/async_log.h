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

#include <assert.h>
#include <strings.h>
#include <string.h>

#include <vector>
#include <memory>
#include <string>


namespace mm {

namespace internal {

enum {
    kLargeBufferLen = 1024 * 1024, // 1MB
    kSmallBufferLen = 1024         // 2KB
};

template <long N = kLargeBufferLen>
class Buffer {
public:
    Buffer()
        : cur_(data_)
    {
        ::bzero(static_cast<void*>(data_), N);
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    ~Buffer() {}

    size_t Len() const { return cur_ - data_; }
    size_t AvailableLen() const { return N - Len(); }
    size_t Append(const char *data, size_t len) {
        //assert(AvailableLen() >= len);
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
    char data_[N];
    char* cur_;
};
}

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF
};

class AsyncLog {
public:
    AsyncLog() : AsyncLog(std::string("")) {}
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

    void SetOutFile(const std::string& file_name) {
        file_name_ = file_name;
    }

    void SetLogLevel(LogLevel log_level) {
        log_level_ = log_level;
    }

    LogLevel log_level() const { return log_level_; }

    size_t writen_nums() const { return writen_nums_; }

private:
    void ThreadFunc();

    MutexLock mutex_;
    MutexCond cond_;
    MutexCond cond_buffer_too_large_;
    Thread th_;

    std::unique_ptr<internal::Buffer<>> buffer_;
    std::vector<std::unique_ptr<internal::Buffer<>>> buffer_vec_;
    std::vector<std::unique_ptr<internal::Buffer<>>> ready_buffer_vec_;


    std::string file_name_;
    bool running_;
    size_t writen_nums_;
    LogLevel log_level_;
};

extern AsyncLog g_async_log;

namespace internal {

class StreamBuffer {
    enum {
        kMaxNumbers = 32,
        kMaxSuffilx = 128
    };

public:
    StreamBuffer(const char* file, int line, const char* func, const LogLevel& level)
        : src_file_(file),
          line_(line),
          func_(func),
          level_(level)
    {
        FormatPrefix();
    }

    StreamBuffer(const StreamBuffer&) = delete;
    StreamBuffer(StreamBuffer&&) = delete;
    StreamBuffer& operator=(const StreamBuffer&) = delete;
    StreamBuffer& operator=(StreamBuffer&&) = delete;

    ~StreamBuffer() {
        FormatSuffix();
        g_async_log.Append(buffer_.data(), buffer_.Len());

    }

    // FIXME: check the buffer_ available length.
    StreamBuffer& operator<<(int v) {
        Sprintf("%d", v);

        return *this;
    }

    StreamBuffer& operator<<(unsigned int v) {
        Sprintf("%u", v);

        return *this;
    }

    StreamBuffer& operator<<(long v) {
        Sprintf("%ld", v);

        return *this;
    }

    StreamBuffer& operator<<(unsigned long v) {
        Sprintf("%lu", v);

        return *this;
    }

    StreamBuffer& operator<<(short v) {
        *this << static_cast<int>(v);
        return *this;
    }

    StreamBuffer& operator<<(unsigned short v) {
        *this << static_cast<unsigned int>(v);
        return *this;
    }

    StreamBuffer& operator<<(double v) {
        Sprintf("%.6lf", v);
        return *this;
    }

    StreamBuffer& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    StreamBuffer& operator<<(char v) {
        buffer_.Append(&v, 1);
        return *this;
    }

    StreamBuffer& operator<<(const char* v) {
        buffer_.Append(v, ::strlen(v));
        return *this;
    }

    StreamBuffer& operator<<(const std::string& v) {
        buffer_.Append(v.c_str(), v.length());
        return *this;
    }

    StreamBuffer& operator<<(void *v) {
        Sprintf("%X", v);
        return *this;
    }

private:
    void FormatPrefix();
    void FormatSuffix();

    template <typename T>
    void Sprintf(const char* fmt, T v) {
        char buf[kMaxNumbers];
        auto l = ::snprintf(buf, kMaxNumbers, fmt, v);
        buffer_.Append(buf, l);
    }

    const char* src_file_;
    int line_;
    const char* func_;
    LogLevel level_;
    Buffer<kSmallBufferLen> buffer_;
};

}


#define LOG_TRACE if (mm::LogLevel::TRACE >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::TRACE)

#define LOG_DEBUG if (mm::LogLevel::DEBUG >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::DEBUG)
#define LOG_INFO if (mm::LogLevel::INFO >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::INFO)
#define LOG_WARN if (mm::LogLevel::WARN >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::WARN)
#define LOG_ERROR if (mm::LogLevel::ERROR >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::ERROR)
#define LOG_FATAL if (mm::LogLevel::FATAL >= mm::g_async_log.log_level()) \
    mm::internal::StreamBuffer(__FILE__, __LINE__, __func__, mm::LogLevel::FATAL)
}

#endif

