#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <unistd.h>
#include <syscall.h>
#include <stdint.h>

namespace sylar {
    /**
     * @brief 获取线程ID
    */
    pid_t GetThreadId(); 

    /**
     * @brief 获取协程ID
    */
   uint32_t GetFiberId();
}

#endif