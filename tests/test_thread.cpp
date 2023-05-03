/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-29 17:58:44
 * @LastEditTime: 2023-04-30 21:47:45
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "sylar/sylar.h"
#include <unistd.h>
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
// sylar::RWMutex s_mutex;
sylar::Mutex s_mutex;
void fun1() {
    SYLAR_LOG_INFO(g_logger)
        << "name: " << sylar::Thread::GetName()
        << " this.name: " << sylar::Thread::GetThis()->getName()
        << " id:" << sylar::GetThreadId()
        << " this.id:" << sylar::Thread::GetThis()->getId();
    for (int i = 0; i < 100000; ++i) {
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while(true) {
        SYLAR_LOG_INFO(g_logger) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    }
}
void fun3() {
    while(true) {
        SYLAR_LOG_INFO(g_logger) << "==================================";
    }
}

int main(int argc, char const *argv[]) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/cxj/workspace/backup/sylar/bin/conf/log2.yml");
    sylar::Config::LoadFromYaml(root);
    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        sylar::Thread::ptr thr1(
            new sylar::Thread(&fun2, "name_" + std::to_string(i * 2)));
        thrs.emplace_back(thr1);
    }

    for (size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    return 0; 
}
