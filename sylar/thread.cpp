/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-28 23:41:11
 * @LastEditTime: 2023-04-29 21:08:37
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "thread.h"
#include "util.h"
#include "log.h"
    
namespace sylar {
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Thread* Thread::GetThis() { return t_thread; }
const std::string& Thread::GetName() { return t_thread_name; }
void Thread::SetName(const std::string& name) {
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}
Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb), m_name(name) {
    if (name.empty()) {
        m_name = "UNKNOW";
    }
    t_thread = this;
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        // 打印系统日志(线程创建失败)
        SYLAR_LOG_ERROR(g_logger)
            << "pthread_create thread fail, tr = " << rt << " name=" << name;
        // TODO: 抛出异常
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}
Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);  // 将线程设为分离态线程, 线程结束系统自动回收
    }
}
void Thread::join() {
    if (m_thread) {
        // 阻塞回收线程
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            // 打印系统日志(线程创建失败)
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, tr = " << rt
                                      << " name=" << m_name;
            // TODO: 抛出异常
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}
void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;  // 静态方法, 显示传入this指针
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();
    cb();
    return 0;
}
}  // namespace sylar