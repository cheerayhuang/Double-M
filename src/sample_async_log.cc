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

mm::AsyncLog g_async_log("mm.log");

const int kMsgLen = 1000 * 1000;
std::string str(kMsgLen, 'h');

const int kLoop = 1000;

void func() {


    //g_async_log.Append(mm::current_thread::GetTidStr(), 6);
    for (auto i = 0; i < kLoop; ++i)
        g_async_log.Append(str.c_str(), str.length());
}

int main() {

    {
    // set max virtual memory to 1GB.
        size_t kOneGB = 1000*1024*1024;
        rlimit rl = { 1*kOneGB, 1*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }


    using namespace mm;
    /*str.insert(0, current_thread::Name());
    str.insert(0, current_thread::GetTidStr());
    */
    str.append(1, '\n');

    auto t1 = mm::Timestamp::Now();
    g_async_log.StartLogging();

    /*mm::Thread th1(func, "th1");
    mm::Thread th2(func, "th2");

    th1.Start();
    th2.Start();

    th1.Join();
    th2.Join();
    */
    func();

    //g_async_log.StopLogging();


    while (g_async_log.writen_nums() < kMsgLen*1*kLoop) {
        struct timeval tv{0, 10};
        ::select(0, NULL, NULL, NULL, &tv);
    }
    auto t2 = mm::Timestamp::Now();

    printf("total bytes: %d, total msgs: %d, total time: %lf\n", kMsgLen*kLoop + kLoop, kLoop, t2.DiffSeconds(t1));
    printf("MB per sec: %lf\n", (kMsgLen*kLoop+kLoop) / t2.DiffSeconds(t1) / (1000*1000));

    return 0;
}
