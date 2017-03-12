/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: thread.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-12-2017 12:36:44
 * @version $Revision$
 *
 **************************************************************************/

#include "current_thread.h"
#include "thread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>


namespace mm {
namespace current_thread {
    __thread int cached_tid;
    __thread char tid_str[32];
    __thread const char* thread_name;

    void CacheTid() {
        if (cached_tid == 0) {
            cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
            int n = snprintf(tid_str, sizeof tid_str, "%5d ", cached_tid);
            assert(n == 6);
            (void)n;
        }
    }

    bool IsMainThread() {
        return GetTid() == ::getpid();
    }

    static const char kMainThreadName[5] = "main";
    static const char kFinishedThreadName[10] = "finished";
    static const char kCrashedThreadName[10] = "crashed";

    // init
    class Init {
    public:
        Init() {
            GetTid();
            thread_name = kMainThreadName;
        }
    };
}
}

using namespace mm;

current_thread::Init init;

std::atomic_uint Thread::num_created_;

Thread::Thread(const ThreadFunc &f, const std::string &name)
    : func_(f),
      name_(name),
      started_(false),
      pthread_id_(0),
      tid_(0)
{
    num_created_++;
}

Thread::~Thread() {
    num_created_--;
}

void Thread::Start() {
    assert(!started_);
    started_ = true;
    errno = pthread_create(&pthread_id_, NULL, &ThreadEntry, this);
    if (errno != 0) {  // errno is thread safe.
        // log error
    }
}

int Thread::Join() {
    assert(started_);
    return pthread_join(pthread_id_, NULL);
}

void* Thread::ThreadEntry(void *obj) {
    auto p = static_cast<Thread*>(obj);
    p->RunThreadFunc();
    return NULL;
}

void Thread::RunThreadFunc() {
    tid_ = current_thread::GetTid();
    current_thread::thread_name = name_.c_str();
    try {
        func_();
        current_thread::thread_name = current_thread::kFinishedThreadName;
    }
    catch (...) {
        // log & handle error
    }
}
