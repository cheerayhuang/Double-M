/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: sample_async_log.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-17-2017 22:44:40
 * @version $Revision$
 *
 **************************************************************************/


#include "async_log.h"
#include "current_thread.h"
#include "thread.h"
#include "timestamp.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/resource.h>

#include <string>

//mm::AsyncLog g_async_log("mm.log");

const size_t kMsgLen = 128;
std::string str(kMsgLen, 'h');

const int kLoop = 2000000;

void func() {

    using namespace mm;
    for (auto i = 0; i < kLoop; ++i)
        LOG_TRACE<<str;
}

int main() {

    {
    // set max virtual memory to 2GB.
        size_t kOneGB = 1024*1024*1024;
        rlimit rl = { 3*kOneGB, 3*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    str += "\n";
    using namespace mm;
    g_async_log.SetOutFile("mm.log");
    g_async_log.SetLogLevel(LogLevel::TRACE);

    auto t1 = mm::Timestamp::Now();
    g_async_log.StartLogging();

    mm::Thread th1(func, "th1");
    mm::Thread th2(func, "th2");

    th1.Start();
    th2.Start();

    auto total_bytes = 2 * ((kMsgLen) * kLoop);
    auto total_msgs = 2 * kLoop;
    while (g_async_log.writen_nums() < total_bytes) {
        struct timeval tv{0, 10};
        ::select(0, NULL, NULL, NULL, &tv);
    }
    auto t2 = mm::Timestamp::Now();

    printf("total bytes: %lu, total msgs: %d, total time: %lf\n", total_bytes, total_msgs, t2.DiffSeconds(t1));
    printf("MB per sec: %lf\n", total_bytes / t2.DiffSeconds(t1) / (1024*1024));
    printf("Msg per sec: %lf\n", total_msgs / t2.DiffSeconds(t1));


    return 0;
}
