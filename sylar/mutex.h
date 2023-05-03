/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-29 20:18:47
 * @LastEditTime: 2023-04-30 22:59:45
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <semaphore.h>
#include <stdint.h>
#include <atomic>
#include <memory>

namespace sylar {
class Semaphore { /*ˈseməˌfôr*/
   public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    /**
     * @brief 获取信号量
     */
    void wait();
    /**
     * @brief 释放信号量
     */
    void notify();

   private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

   private:
    sem_t m_semaphore;
};

/**
 * @brief 局部锁的模板实现
*/
template<class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }
private:
    ScopedLockImpl(const ScopedLockImpl&) = delete;
    ScopedLockImpl(const ScopedLockImpl&&) = delete;
    ScopedLockImpl& operator=(const ScopedLockImpl&) = delete;
private:
    /// @brief 互斥量
    T& m_mutex;
    /// @brief 查看是否上锁
    bool m_locked;
};

class Mutex {
public:
    typedef ScopedLockImpl<Mutex> Lock;
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class NullMutex {
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}
};

class Spinlock {
public:
    typedef ScopedLockImpl<Spinlock> Lock;
    Spinlock() {
        pthread_spin_init(&m_mutex, 0); // 非进程间共享
    }
    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }
    void lock() {
        pthread_spin_lock(&m_mutex);
    }
    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

/**
 * @brief 局部读锁的模板实现
*/
template<class T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }
private:
    ReadScopedLockImpl(const ReadScopedLockImpl&) = delete;
    ReadScopedLockImpl(const ReadScopedLockImpl&&) = delete;
    ReadScopedLockImpl& operator=(const ReadScopedLockImpl&) = delete;
private:
    /// @brief 互斥量
    T& m_mutex;
    /// @brief 查看是否上锁
    bool m_locked;
};

/**
 * @brief 局部写锁的模板实现
*/
template<class T>
class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }
private:
    WriteScopedLockImpl(const WriteScopedLockImpl&) = delete;
    WriteScopedLockImpl(const WriteScopedLockImpl&&) = delete;
    WriteScopedLockImpl& operator=(const WriteScopedLockImpl&) = delete;
private:
    /// @brief 互斥量
    T& m_mutex;
    /// @brief 查看是否上锁
    bool m_locked;
};

class RWMutex {
public:
    /// @brief 局部读锁
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    /// @brief 局部写锁
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }
    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }
    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    } 
    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }
    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

class CASLock {
public:
    typedef ScopedLockImpl<CASLock> Lock;
    CASLock() {
        m_mutex.clear();
    }
    ~CASLock() {

    }
    void lock() {
        while(m_mutex.test_and_set(std::memory_order_acquire)) { // 如果A跟V不一致, 返回true
            // 自旋等待自旋锁被释放
        }
    }   
    void unlock() {
        m_mutex.clear(std::memory_order_release); // 释放自旋锁
    }
private:
    /// @brief 原子状态, 直接从内存读取
    volatile std::atomic_flag m_mutex;
};
}  // namespace sylar
#endif  //__MUTEX_H__