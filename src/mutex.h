/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: mutex.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-12-2017 14:34:26
 * @version $Revision$
 *
 **************************************************************************/



#ifndef __DOUBLE_M_MUTEX_H__
#define __DOUBLE_M_MUTEX_H__

#include "current_thread.h"

#include <assert.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

namespace mm {

class MutexLock {
public:
    MutexLock()
        : holder_(0)
    {
        errno = pthread_mutex_init(&mutex_, NULL);
        if (errno != 0) {
            // log error
        }
    }

    MutexLock(const MutexLock&) = delete;
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

    ~MutexLock() {
        assert(holder_ == 0);
        errno = pthread_mutex_destroy(&mutex_);
        if (errno != 0) {
            // log error
        }
    }

private:
    void Lock() {
        pthread_mutex_lock(&mutex_);
        holder_ = current_thread::GetTid();
    }

    void Unlock() {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* GetLock() {
        return &mutex_;
    }

    pthread_mutex_t mutex_;
    pid_t holder_;

friend class MutexLockGuard;
friend class MutexCond;

};

class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& m)
        : mutex_(m)
    {
        mutex_.Lock();
    }

    MutexLockGuard(const MutexLockGuard&) = delete;
    MutexLockGuard(MutexLockGuard&&) = delete;
    MutexLockGuard& operator=(const MutexLockGuard&) = delete;
    MutexLockGuard& operator=(MutexLockGuard&&) = delete;

    ~MutexLockGuard() {
        mutex_.Unlock();
    }

private:
    MutexLock& mutex_;
};
// prevent to create a tmp MutexLockGuard, like: "MutexLockGuard(m);"
#define MutexLockGuard(x) assert(false)

class MutexCond {
public:
    MutexCond(MutexLock &mutex)
        : mutex_(mutex)
    {
        errno = pthread_cond_init(&cond_, NULL);
        if (errno != 0) {
            // log error
        }
    }

    MutexCond(const MutexCond&) = delete;
    MutexCond(MutexCond&&) = delete;
    MutexCond& operator=(const MutexCond&) = delete;
    MutexCond& operator=(MutexCond&&) = delete;

    ~MutexCond() {
        errno = pthread_cond_destroy(&cond_);
        if (errno != 0) {
            // log error
        }
    }

    void Wait() {
        pthread_cond_wait(&cond_, mutex_.GetLock());
    }

    bool WaitOrTimeout(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += seconds;
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.GetLock(), &abstime);

    }

    void Notify() {
        pthread_cond_signal(&cond_);
    }

    void NotifyAll() {
        pthread_cond_broadcast(&cond_);
    }

private:
    // since mutex_.Lock() can't be called, here is a reference
    MutexLock& mutex_;

    pthread_cond_t cond_;
};

}

#endif

