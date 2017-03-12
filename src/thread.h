/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: Thread.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-09-2017 21:53:05
 * @version $Revision$
 *
 **************************************************************************/

#ifndef __DOUBLE_M_THREAD_H__
#define __DOUBLE_M_THREAD_H__


#include <atomic>
#include <functional>
#include <string>

#include <pthread.h>

namespace mm {
class Thread {

public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc &f, const std::string &name = std::string());

    Thread(Thread &) = delete;
    Thread(Thread &&) = delete;
    Thread& operator=(const Thread &) = delete;
    Thread& operator=(Thread&&) = delete;

    ~Thread();

    void Start();
    int Join();

private:
    void RunThreadFunc();
    static void* ThreadEntry(void *obj);

    ThreadFunc func_;
    std::string name_;
    bool started_;
    pthread_t pthread_id_;  // id from lib pthread
    pid_t tid_;             // id generated from gettid(2)

    static std::atomic_uint num_created_;
};
}
#endif

