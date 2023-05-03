/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-30 12:43:53
 * @LastEditTime: 2023-04-30 12:56:57
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include <atomic>
#include <iostream>
#include <vector>

#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
int count = 1000000;
bool flag = false;
std::atomic_flag winner = ATOMIC_FLAG_INIT;
void fun1() {
    while(!flag) {

    }
    for (int i = 0; i < count; i ++) {

    }
    if (!winner.test_and_set()) {
        SYLAR_LOG_INFO(g_logger) << pthread_self();
    }
    while(1) sleep(1);
}
int main(int argc, char const *argv[])
{
    // 创建10个线程， 率先完成计数任务的线程输出自己的id, 后续完成的不输出
    std::vector<sylar::Thread::ptr> threads;
    SYLAR_LOG_INFO(g_logger) << "atomic test begin";
    for (int i = 0; i < 10; ++i) {
        sylar::Thread::ptr thr1(new sylar::Thread(&fun1, "name_" + std::to_string(i)));
        threads.emplace_back(thr1);
    }
    flag = true;
    for (size_t i = 0; i < threads.size(); i ++) {
        threads[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "atomic test end";
    return 0;
}
