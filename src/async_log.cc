/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: async_log.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-17-2017 19:19:55
 * @version $Revision$
 *
 **************************************************************************/

#include "async_log.h"
#include "log_file.h"
#include "timestamp.h"

#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

#include <functional>

using namespace mm;

AsyncLog::AsyncLog(const std::string& file_name)
    : th_(std::bind(&AsyncLog::ThreadFunc, this), "AsyncLogging"),
      mutex_(),
      cond_(mutex_),
      cond_buffer_too_large_(mutex_),
      buffer_(new internal::Buffer<>),
      file_name_(file_name),
      running_(false),
      writen_nums_(0),
      log_level_(LogLevel::WARN)
{
}

AsyncLog::~AsyncLog() {
    StopLogging();
}

void AsyncLog::StartLogging() {
    running_ = true;
    th_.Start();
}

void AsyncLog::StopLogging() {
    running_ = false;
    th_.Join();
}

void AsyncLog::Append(const std::string& msg, size_t len) {
    Append(msg.c_str(), len);
}

void AsyncLog::Append(const char* msg, size_t len) {
    MutexLockGuard lock(mutex_);
    while (buffer_vec_.size() > 100) {
        cond_buffer_too_large_.Wait();
    }

    size_t index = 0;
    while (len > 0) {
        auto real_len = buffer_->AvailableLen();
        if (real_len == 0) {
            buffer_vec_.push_back(std::unique_ptr<internal::Buffer<>>(buffer_.release()));
            buffer_.reset(new internal::Buffer<>);
            real_len = buffer_->AvailableLen();
        }
        if (real_len > len) {
            real_len = len;
        }
        buffer_->Append(msg + index, real_len);
        len -= real_len;
        index += real_len;
    }

    if (!buffer_vec_.empty()) {
        // notify consumer immediately. avoid OS free the memory.
        cond_.Notify();
    }
}

void AsyncLog::ThreadFunc() {
    LogFile logfile(file_name_);

    while (running_) {
        {
            MutexLockGuard lock(mutex_);

            if (buffer_vec_.empty() && !buffer_->Empty()) {
                buffer_vec_.push_back(std::unique_ptr<internal::Buffer<>>(buffer_.release()));
                buffer_.reset(new internal::Buffer<>);
            }

            if (buffer_vec_.empty()) {
                cond_.WaitOrTimeout(1);
            }

            // if buffer is still empty, dont need to do next steps.
            if (buffer_vec_.empty()) {
                continue;
            }

            assert(!buffer_vec_.empty());
            buffer_vec_.swap(ready_buffer_vec_);
            cond_buffer_too_large_.NotifyAll();
        }

        auto c_iter = ready_buffer_vec_.cbegin();
        for (; c_iter != ready_buffer_vec_.cend(); ++c_iter) {
            writen_nums_ += (*c_iter)->Len();
            logfile.Write((*c_iter)->data(), (*c_iter)->Len());
        }

        ready_buffer_vec_.clear();
        ready_buffer_vec_.shrink_to_fit();
    }

    logfile.Flush();
}

__thread char t_time_str[32];
__thread time_t t_lastsec = 0;

namespace mm {

AsyncLog g_async_log;

namespace internal {

    void StreamBuffer::FormatPrefix() {
        auto now = Timestamp::Now();
        auto epoch_us = now.epoch_us();

        time_t seconds = epoch_us / Timestamp::kMicrosecondsPerSecond;
        int us = epoch_us % Timestamp::kMicrosecondsPerSecond;
        if (seconds != t_lastsec) {
            t_lastsec = seconds;
            struct tm tm_time;
            ::gmtime_r(&seconds, &tm_time);
            strftime(t_time_str, sizeof(t_time_str), "%Y-%m-%dT%T", &tm_time);
        }
        char us_str[16];
        snprintf(us_str, sizeof(us_str), ".%06dZ ", us);
        assert(::strlen(t_time_str) == 19);

        buffer_.Append(t_time_str, 19);
        buffer_.Append(us_str, 9);
    }


    void StreamBuffer::FormatSuffix() {
        static const char kPattern[] = " - %s:%d:%s()\n";
        char buf[kMaxSuffilx];
        auto l = snprintf(buf, sizeof(buf), kPattern, src_file_, line_, func_);

        buffer_.Append(buf, l);
    }
}
}

