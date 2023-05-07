/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-07 10:31:20
 * @LastEditTime: 2023-05-08 00:01:58
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace sylar {
/**
 * @brief 基于EPOLL的IO协程调度器
 * 
 */
class IOManager : public Scheduler, public TimeManager {
public:
	typedef std::shared_ptr<IOManager> ptr;
	typedef RWMutex RWMutexType;
    /**
     * @brief IO事件
     */
	enum Event {    
		NONE = 0x0,  // 无事件
		READ = 0x1,  // EPOLLIN 读事件
		WRITE = 0x4, // EPOLLOUT 写事件
	};

private:
    /**
     * @brief Socket事件上下文
     */
	struct FdContext {
		typedef Mutex MutexType;
		struct EventContext {
			Scheduler* scheduler = nullptr; // 事件执行的scheduler
			Fiber::ptr fiber;               // 事件协程
			std::function<void()> cb;       // 事件的回调函数
		};
		/**
		 * @brief 获取事件上下文对象
		 * @param[in] event 事件类型
		 * @return 
		 */
		EventContext& getContext(Event event);
		void resetContext(EventContext& ctx);
		void triggerEvent(Event event);

		EventContext read;   // 读事件
		EventContext write;  // 写事件
		int fd = 0;          // 事件关联的句柄
		Event events = NONE; // 已经注册的事件
		MutexType mutex;
	};

public:
	IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "SchedulerThread");
	~IOManager();

	// 0 success, -1 error
	int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
	bool delEvent(int fd, Event event);
	bool cancelEvent(int fd, Event event);

	bool cancelAll(int fd);

	static IOManager* GetThis();

protected:
	void tickle() override;
	bool stopping() override;
	void idle() override;
	void onTimerInsertedAtFront() override;

	bool stopping(uint64_t& timeout);

	void contextResize(size_t size);

private:
	/**
	 * @brief epoll文件描述符
	 */
	int m_epfd = 0;
	int m_tickleFds[2];
	/// 当前等待执行的事件数量
	std::atomic<size_t> m_pendingEventCount = {0};	
	RWMutexType m_mutex;
	std::vector<FdContext*> m_fdContexts;
};

} // namespace sylar

#endif //__IOMANAGER_H__