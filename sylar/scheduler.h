/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-03 20:22:44
 * @LastEditTime: 2023-05-08 00:00:50
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "fiber.h"
#include "mutex.h"
#include "thread.h"
#include <list>
#include <memory>
#include <vector>

namespace sylar {
class Scheduler {
public:
	typedef std::shared_ptr<Scheduler> ptr;
	typedef Mutex MutexType;
	/**
	 * @brief Construct a new Scheduler object
	 * @param[in] threads 线程数量
	 * @param[in] use_caller 是否使用当前调用线程
	 * @param[in] name 协程调度器名称
	 */
	Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "SchedulerThread");
	virtual ~Scheduler();

public:
	static Scheduler* GetThis();
	static Fiber* GetMainFiber();

public:
	const std::string& getName() const { return m_name; }
	bool hasIdleThreads() { return m_idleThreadCount > 0; }

public:
	void start();
	void stop();
	template <class FiberOrCb>
	void schedule(FiberOrCb fc, int thread = -1) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			need_tickle = scheduleNoLock(fc, thread);
		}
		if (need_tickle) {
			tickle();
		}
	}
	template <class InputIterator>
	void schedule(InputIterator begin, InputIterator end) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			while (begin != end) {
				need_tickle = scheduleNoLock(&*begin, -1) || need_tickle; // 循环向任务队列中添加任务
				++begin;
			}
		}
		if (need_tickle) {
			tickle();
		}
	}

protected:
	virtual void tickle();
    /**
     * @brief 返回是否可以停止
     */
	virtual bool stopping();
	
	virtual void idle();
protected:
	/**
	 * @brief 协程调度函数
	 */
    void run();
    /**
     * @brief 设置当前的协程调度器
     */
    void setThis();
private:
	template <class FiberOrCb>
	bool scheduleNoLock(FiberOrCb fc, int thread) {
		bool need_tickle = m_fibers.empty();
		FiberAndThread ft(fc, thread);
		if (ft.fiber || ft.cb) {
			m_fibers.push_back(ft);
		}
		return need_tickle;
	}

private:
	struct FiberAndThread {
		Fiber::ptr fiber;         /// 协程
		std::function<void()> cb; /// 协程执行函数
		int thread;               /// 线程id
		/**
		 * @brief 构造函数
		 * @param[in] f 协程
		 * @param[in] thr 线程id
		 */
		FiberAndThread(Fiber::ptr f, int thr)
		    : fiber(f)
		    , thread(thr) {}
		/**
		 * @brief 构造函数
		 * @param[in] f 协程指针
		 * @param[in] thr 线程id
		 * @post *f = nullptr
		 */
		FiberAndThread(Fiber::ptr* f, int thr)
		    : thread(thr) {
			fiber.swap(*f);
		}
		/**
		 * @brief 构造函数
		 * @param[in] f 协程执行函数
		 * @param[in] thr 线程id
		 */
		FiberAndThread(std::function<void()> f, int thr)
		    : cb(f)
		    , thread(thr) {}
		/**
		 * @brief 构造函数
		 * @param[in] f 协程执行函数指针
		 * @param[in] thr 线程id
		 * @post *f = nullptr
		 */
		FiberAndThread(std::function<void()>* f, int thr)
		    : thread(thr) {
			cb.swap(*f);
		}
		/**
		 * @brief 无参构造函数
		 */
		FiberAndThread()
		    : thread(-1) {}
		/**
		 * @brief 重置数据
		 */
		void reset() {
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}
	};

private:
	MutexType m_mutex;                  /// 互斥量
	std::vector<Thread::ptr> m_threads; /// 线程池
	std::list<FiberAndThread> m_fibers; /// 待执行的任务队列
	std::string m_name;                 /// 协程调度器名称
	Fiber::ptr m_rootFiber;             /// 主协程
protected:
	std::vector<int> m_threadIds;       /// 线程id列表
	size_t m_threadCount = 0;           /// 线程数量
	size_t m_activeThreadCount = 0;     /// 活跃线程数量
	size_t m_idleThreadCount = 0;       /// 空闲线程数量
	bool m_stopping = true;             /// 是否正在停止
	bool m_autostop = false;            /// 是否自动停止
	int m_rootThread = 0;               /// 主线程id(use_caller)
};
} // namespace sylar

#endif //__SCHEDULER_H__