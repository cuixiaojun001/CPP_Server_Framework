/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-29 17:58:44
 * @LastEditTime: 2023-04-29 23:30:01
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "sylar/sylar.h"
#include <unistd.h>
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
sylar::RWMutex s_mutex;
void fun1() {
    SYLAR_LOG_INFO(g_logger)
        << "name: " << sylar::Thread::GetName()
        << " this.name: " << sylar::Thread::GetThis()->getName()
        << " id:" << sylar::GetThreadId()
        << " this.id:" << sylar::Thread::GetThis()->getId();
    for (int i = 0; i < 100000; ++i) {
        sylar::RWMutex::WriteLock lock(s_mutex);
        ++count;
    }
}

void fun2() {}

int main(int argc, char const *argv[]) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        sylar::Thread::ptr thr(
            new sylar::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.emplace_back(thr);
    }

    for (int i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count=" << count;
    return 0; 
}
