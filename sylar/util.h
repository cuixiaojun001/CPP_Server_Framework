/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-29 17:58:44
 * @LastEditTime: 2023-05-07 22:38:09
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>

namespace sylar {

pid_t GetThreadId();
uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size, int skip);
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "    ");
/**
 * @brief 获取当前时间戳(毫秒)
 * @return 返回时间戳
 */
uint64_t GetCurrentMs();
} // namespace sylar

#endif
