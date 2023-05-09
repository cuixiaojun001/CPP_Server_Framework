/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-03 20:23:05
 * @LastEditTime: 2023-05-09 14:15:51
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include "hook.h"

namespace sylar {
/*-----------全局变量-----------*/
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_scheduler_fiber = nullptr;

/*-----------构造/析构-----------*/
Scheduler::Scheduler(size_t threads /*=1*/, bool use_caller /*=true*/, const std::string& name /*=""*/)
    : m_name(name) {
	SYLAR_ASSERT1(threads > 0);
	if (use_caller) { // caller线程也被加入到调度线程池
		sylar::Fiber::GetThis();
		--threads;

		SYLAR_ASSERT1(GetThis() == nullptr);
		t_scheduler = this;
		m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));
		sylar::Thread::SetName(m_name);
		t_scheduler_fiber = m_rootFiber.get();
		m_rootThread = sylar::GetThreadId();
		m_threadIds.push_back(m_rootThread); // 在线程中声明调度器, 并且将该线程放到调度器中, 主协程不是线程主协程, 而是执行run() 的主协程
	} else {
		m_rootThread = -1;
	}
	m_threadCount = threads;
}
Scheduler::~Scheduler() {
	SYLAR_LOG_DEBUG(g_logger) << "Scheduler::~Scheduler()";
	SYLAR_ASSERT1(m_stopping);
	if (GetThis() == this) {
		t_scheduler = nullptr;
	}
}
/*-----------静态方法-----------*/
Scheduler* Scheduler::GetThis() {
	return t_scheduler;
}
Fiber* Scheduler::GetMainFiber() {
	return t_scheduler_fiber;
}
/*-----------公有成员方法-----------*/
void Scheduler::start() {
	MutexType::Lock lock(m_mutex);
	if (!m_stopping) {
		SYLAR_LOG_ERROR(g_logger) << "调度器未启动";
		return; // 调度器未启动
	}
	SYLAR_ASSERT1(m_threads.empty());
	m_threads.resize(m_threadCount);
	SYLAR_LOG_DEBUG(g_logger) << "调度线程开始调度, m_threadCount= " << m_threadCount;
	for (size_t i = 0; i < m_threadCount; i++) {
		m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i + 1)));
		m_threadIds.emplace_back(m_threads[i]->getId()); // 在线程ctor中引入信号量机制, 以确保完成构造前线程id被正确的初始化
	}
	lock.unlock();
}
void Scheduler::stop() {
	SYLAR_LOG_INFO(g_logger) << "Scheduler::stop() 调度结束";
	m_autostop = true;
	if (m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
		SYLAR_LOG_INFO(g_logger) << this << "stopped";
		m_stopping = true;
		if (stopping()) {
			SYLAR_LOG_INFO(g_logger) << "当前没有任何任务未完成, 退出调度器, 程序走向结束";
			return;
		}
	}
	if (m_rootThread != -1) {             // 使用了use_caller线程
		SYLAR_ASSERT1(GetThis() == this); // 必须在use_caller线程执行stop()
	} else {                              // 没使用use_caller线程
		SYLAR_ASSERT1(GetThis() != this);
	}

	m_stopping = true;
	for (size_t i = 0; i < m_threadCount; i++) {
		tickle(); // 唤醒每一个线程
	}

	if (m_rootFiber) {
		tickle();
	}

	if (m_rootFiber) {
		SYLAR_LOG_DEBUG(g_logger) << "use_caller = true threadCount = 1, caller线程开始调度";
		m_rootFiber->swapIn(); // 调度线程只有caller线程, swapIn() 进入调度协程
	}

	std::vector<Thread::ptr> thrs;
	{
		MutexType::Lock lock(m_mutex);
		thrs.swap(m_threads);
	}
	for (auto& i : thrs) {
		i->join();
	}

	if (stopping())
		return;
	SYLAR_LOG_DEBUG(g_logger) << "Scheduler::stop() end";
}

/*-----------保护成员方法-----------*/
void Scheduler::run() { // 执行调度
	SYLAR_LOG_INFO(g_logger) << "run()";
	set_hook_enable(true); // 调度线程开启hook
	setThis();
	if (sylar::GetThreadId() != m_rootThread) {                                    // 当前线程不是调度线程
		t_scheduler_fiber = Fiber::GetThis().get();                                // 调度协程等于当前协程
	}
	Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this), 0, true)); // 空闲协程
	Fiber::ptr cb_fiber;
	FiberAndThread ft;
	while (true) {
		SYLAR_LOG_DEBUG(g_logger) << "开始调度";
		ft.reset();
		bool tickle_me = false;
		{
			MutexType::Lock lock(m_mutex);
			auto it = m_fibers.begin();
			while (it != m_fibers.end()) {
				if (it->thread != -1 && it->thread != sylar::GetThreadId()) {
					// 该调度任务绑定线程, 但不是该执行run()方法的线程, 对该任务不做处理
					++it;
					tickle_me = true;
					continue;
				}
				SYLAR_ASSERT1(it->fiber || it->cb);
				if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
					// 该调度任务已经处于执行状态
					++it;
					continue;
				}
				ft = *it;
				m_fibers.erase(it++);
				break;
			}
		}
		if (tickle_me) {
			tickle();
		}
		if (ft.fiber && (ft.fiber->getState() != Fiber::TERM || ft.fiber->getState() != Fiber::EXCEPT)) {
			// 调度任务为协程
			++m_activeThreadCount;
			ft.fiber->swapIn();
			--m_activeThreadCount;
			SYLAR_LOG_DEBUG(g_logger) << "当前协程任务执行完毕";
			// if (ft.fiber->getState() == Fiber::READY) {
			// 	// 协程状态为可执行状态
			// 	schedule(ft.fiber);
			// } else if (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT) {
			// 	// 协程状态为INIT, EXEC, HOLD, 任务未结束, 让出执行时间, 将状态设为HOLD
			// 	ft.fiber->m_state = Fiber::HOLD;
			// }
			ft.reset();
		} else if (ft.cb) {                                 // 调度任务为函数
			if (cb_fiber) {
				SYLAR_LOG_DEBUG(g_logger) << "cb_fiber 存在";
				cb_fiber->reset(ft.cb); // 封装函数为协程
			} else {
				SYLAR_LOG_DEBUG(g_logger) << "cb_fiber 不存在";
				cb_fiber.reset(new Fiber(ft.cb, 0, true)); // 封装函数为协程
			}
			ft.reset();
			++m_activeThreadCount;
			cb_fiber->swapIn();
			--m_activeThreadCount;
			SYLAR_LOG_DEBUG(g_logger) << "当前函数任务执行完毕";
			cb_fiber.reset(); // 删掉该任务协程
			// if (cb_fiber->getState() == Fiber::READY) {
			// 	schedule(cb_fiber);
			// 	cb_fiber.reset();
			// } else if (cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM) {
			// 	cb_fiber->reset(nullptr);
			// } else { // if(cb_fiber->getState() != Fiber::TERM) {
			// 	cb_fiber->m_state = Fiber::HOLD;
			// 	cb_fiber.reset();
			// }
		} else { // 没有调度任务
			/* switch (idle_fiber->getState())
			{
			case Fiber::TERM:
				SYLAR_LOG_DEBUG(g_logger) << "当前没有调度任务, idle协程状态为 TERM";
				break;
			case Fiber::INIT:
				SYLAR_LOG_DEBUG(g_logger) << "当前没有调度任务, idle协程状态为 INIT";
				break;
			case Fiber::HOLD:
				SYLAR_LOG_DEBUG(g_logger) << "当前没有调度任务, idle协程状态为 HOLD";
				break;	
			case Fiber::EXEC:
				SYLAR_LOG_DEBUG(g_logger) << "当前没有调度任务, idle协程状态为 EXEC";
				break;
			default:
				SYLAR_LOG_DEBUG(g_logger) << "当前没有调度任务, idle协程状态为 UNKNOWN";
				break;
			} */
			if (idle_fiber->getState() == Fiber::TERM) {
				SYLAR_LOG_INFO(g_logger) << "idle_fiber TREM 当前无任务";
				break;
			}
			++m_idleThreadCount;
			idle_fiber->swapIn();
			--m_idleThreadCount;
			if(idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;
            }
		}
	}
	SYLAR_LOG_INFO(g_logger) << "run() end";
}

void Scheduler::setThis() {
	t_scheduler = this;
}
/*-----------虚函数-----------*/
bool Scheduler::stopping() {
	MutexType::Lock lock(m_mutex);
	return m_autostop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}
void Scheduler::tickle() {
	// SYLAR_LOG_INFO(g_logger) << "tickle";
}
void Scheduler::idle() {
	SYLAR_LOG_INFO(g_logger) << "调度协程idle, 当前无任务";
}
} // namespace sylar
