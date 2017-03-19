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

#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include <functional>
#include <string>

using namespace mm;

AsyncLog::AsyncLog(const std::string& file_name)
    : th_(std::bind(&AsyncLog::ThreadFunc, this), "AsyncLogging"),
      mutex_(),
      cond_(mutex_),
      buffer_(new Buffer),
      ready_buffer_(new Buffer),
      file_name_(file_name),
      running_(false),
      writen_nums_(0)
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

    while (buffer_->AvailableLen() < len) {
        cond_.Wait();
    }
    assert(buffer_->AvailableLen() >= len);

    while (len > 0) {
        auto n = buffer_->Append(msg, len);
        assert(n > 0);
        len -= n;
    }
}

void AsyncLog::ThreadFunc() {
    LogFile logfile(file_name_);

    while (running_) {
        {
            MutexLockGuard lock(mutex_);
            if (buffer_->Empty()) {

                /*struct timeval tv{0, 1000*1000};
                ::select(0, NULL, NULL, NULL, &tv);
                continue;
                */
                cond_.WaitOrTimeout(1);
            }
            buffer_.swap(ready_buffer_);
            cond_.NotifyAll();
        }

        writen_nums_ += ready_buffer_->len();
        logfile.Write(ready_buffer_->data(), ready_buffer_->len());

        ready_buffer_->Clear();
        logfile.Flush();
    }
    logfile.Flush();
}

