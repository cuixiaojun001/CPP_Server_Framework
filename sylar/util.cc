/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-29 17:58:44
 * @LastEditTime: 2023-05-07 22:43:49
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "util.h"
#include "log.h"
#include "fiber.h"

#include <execinfo.h>
#include <sys/time.h>

namespace sylar {
sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return sylar::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** buffer = (void**)malloc(sizeof(void*) * size);
    size_t s = ::backtrace(buffer, size);
    char** strings = backtrace_symbols(buffer, s);
    if (strings == NULL) {
        SYLAR_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }
    for (size_t i = skip; i < s; ++i) {
        bt.emplace_back(strings[i]);
    }
    free(strings);
    free(buffer);

}
std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}


uint64_t GetCurrentMs() { 
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
}

}
