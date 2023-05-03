/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-03 10:37:11
 * @LastEditTime: 2023-05-03 20:02:52
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "sylar/sylar.h"
#include <iostream>
#include <vector>
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber() begin";
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber()   end";
    sylar::Fiber::YieldToHold();
}

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "main begin";
    {
        sylar::Fiber::GetThis();     // 创建主协程
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber)); // 创建子协程, 协程入口函数是run_in_fiber
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after swapin";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    SYLAR_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char const *argv[]) 
{
    sylar::Thread::SetName("main");
    std::vector<sylar::Thread::ptr> threads;
    for (int i = 0; i < 3; i++) {
        threads.emplace_back(sylar::Thread::ptr(new sylar::Thread(&test_fiber, "name_" + std::to_string(i))));
    }

    for (auto i : threads) {
        i->join();
    }
    return 0;
}
