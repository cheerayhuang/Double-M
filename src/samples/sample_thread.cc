/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: main.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-12-2017 13:01:11
 * @version $Revision$
 *
 **************************************************************************/

#include "thread.h"
#include "current_thread.h"
#include "mutex.h"

#include <unistd.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <memory>

using namespace std;

vector<int> g_vec;

mm::MutexLock mutex;
mm::MutexCond cond(mutex);

void func() {
    cout << mm::current_thread::GetTid() << ": " << mm::current_thread::Name() <<  endl;

    mm::MutexLockGuard Lock(mutex);
    cond.Wait();
    cout << "th1 wake up" << endl;
    g_vec.push_back(10);
}

void func2() {
    cout << mm::current_thread::GetTid() << ": " << mm::current_thread::Name() <<  endl;
    mm::MutexLockGuard Lock(mutex);
    cond.Wait();
    cout << "th2 wake up" << endl;
    g_vec.push_back(11);
}

int main() {
    mm::Thread th(func, "th1");
    mm::Thread th2(func2, "th2");
    th.Start();
    th2.Start();
    sleep(3);
    cond.NotifyAll();
    th.Join();
    th2.Join();
    cout << mm::current_thread::GetTid() << ": " << mm::current_thread::Name() << " !thread will end" << endl;

    for (auto i : g_vec) {
        cout << i << endl;
    }

    /*vector<int> local_vec;

    ostream_iterator<int> out_iter(cout, "\n");
    //copy(g_vec.begin(), g_vec.end(), inserter(local_vec, local_vec.begin()));
    local_vec.insert(local_vec.begin(), g_vec[0]);
    local_vec.insert(local_vec.begin(), g_vec[1]);

    copy(local_vec.begin(), local_vec.end(), out_iter);
    */

    return 0;
}

