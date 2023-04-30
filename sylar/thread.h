/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-28 23:02:08
 * @LastEditTime: 2023-04-29 21:09:20
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __THREAD_H__
#define __THREAD_H__

#include <functional>
#include <string>
#include <pthread.h>
#include "mutex.h"

namespace sylar {
    
    class Thread {
        public:
            typedef std::shared_ptr<Thread> ptr;
            typedef pid_t tid_t;
            Thread(std::function<void()> cb, const std::string& name);
            ~Thread();
        public:
            /**
             * @brief 回收线程
            */
            void join();
            /**
             * @brief 获取当前线程
            */
            static Thread* GetThis();
            /**
             * @brief 获取当前线程的名称
            */
            static const std::string& GetName();
            /**
             * @brief 设置线程名称
            */
            static void SetName(const std::string& name);
        public:
            const std::string& getName() const { return m_name;}
            pthread_t getId() const { return m_id;}
        private:
            /*禁止拷贝构造, 移动构造, 拷贝赋值*/
            Thread(const Thread&) = delete;
            Thread(const Thread&&) = delete;
            Thread& operator=(const Thread&) = delete;

            static void* run(void* arg);
        private:
            tid_t m_id; // 内核实际的线程ID
            pthread_t m_thread;
            std::function<void()> m_cb;
            std::string m_name;
            Semaphore m_semaphore;
    };
}

#endif //__THREAD_H__