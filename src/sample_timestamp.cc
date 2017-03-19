/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: sample_timestamp.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-20-2017 00:15:18
 * @version $Revision$
 *
 **************************************************************************/

#include "timestamp.h"
#include <iostream>

using namespace std;

int main() {
    mm::func();

    auto t = mm::Timestamp::Now();
    cout << mm::t2.DiffMicroSeconds(mm::t) << endl;
    auto t2 = mm::Timestamp::Now();
    cout << t2.DiffSeconds(t) << endl;


    return 0;
}

