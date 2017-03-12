/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: Thread.cc
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 03-10-2017 16:11:15
 * @version $Revision$
 *
 **************************************************************************/

#ifndef __DOUBLE_M_CURRENT_THREAD_H__
#define __DOUBLE_M_CURRENT_THREAD_H__

namespace mm {
namespace current_thread {
  // internal
  extern __thread int cached_tid;
  extern __thread char tid_str[32];
  extern __thread const char* thread_name;

  void CacheTid();
  bool IsMainThread();

  inline int GetTid() {
    if (cached_tid == 0)
    {
      CacheTid();
    }
    return cached_tid;
  }

  inline const char* GetTidStr() {
    return tid_str;
  }

  inline const char* Name() {
    return thread_name;
  }

}
}

#endif

