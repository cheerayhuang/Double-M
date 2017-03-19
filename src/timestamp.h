/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: timestamp.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-19-2017 23:44:13
 * @version $Revision$
 *
 **************************************************************************/

#include <sys/time.h>
#include <stdint.h>

#include <utility>

namespace mm {

class Timestamp {
private:
    enum { kMicrosecondsPerSecond = 1000 * 1000 };

public:
    explicit Timestamp(int64_t microseconds)
        : epoch_us_(microseconds)
    {
    }

    Timestamp(const Timestamp& t)
        : epoch_us_(t.epoch_us_)
    {
    }

    // delegate copy-ctor.
    Timestamp(Timestamp&& t) : Timestamp(t) {}

    Timestamp& operator=(const Timestamp &t) {
        epoch_us_ = t.epoch_us_;

        return *this;
    }

    Timestamp& operator=(Timestamp &&t) {
        *this = t;

        return *this;
    }

    ~Timestamp() = default;

    static Timestamp Now() {
        struct timeval tv;
        ::gettimeofday(&tv, nullptr);
        int64_t seconds = tv.tv_sec;

        return Timestamp(seconds * kMicrosecondsPerSecond + tv.tv_usec);
    }

    int64_t DiffMicroSeconds(const Timestamp &t) {
        return epoch_us_ - t.epoch_us_;
    }

    double DiffSeconds(const Timestamp &t) {
        int64_t diff_usecs = epoch_us_ - t.epoch_us_;
        double diff_secs = diff_usecs / static_cast<double>(kMicrosecondsPerSecond);

        return diff_secs;
    }


private:
    int64_t epoch_us_;
};
}

