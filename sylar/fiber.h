/*
 * @brief 协程封装
 * @Author: Cui XiaoJun
 * @Date: 2023-04-30 21:12:53
 * @LastEditTime: 2023-05-05 20:13:42
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __FIBER_H__
#define __FIBER_H__

#include <atomic>
#include <functional>
#include <memory>
#include <stdint.h>
#include <ucontext.h>

namespace sylar {
class Scheduler;
class Fiber : public std::enable_shared_from_this<Fiber> {
	friend Scheduler;
public:
	typedef std::shared_ptr<Fiber> ptr;
	enum State {
		INIT,  /// 初始化状态
		HOLD,  /// 暂停状态
		EXEC,  /// 执行状态
		TERM,  /// 结束状态
		READY, /// 可执行状态
		EXCEPT /// 异常状态
	};

private:
	/**
	 * @brief 主协程的ctor
	 */
	Fiber();

public:
	/**
	 * @brief 构造函数
	 * @param[in] cb 协程执行的函数
	 * @param[in] stacksize 协程栈大小
	 * @param[in] run_in_scheduler 是否参与调度
	 */
	Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = false);
	~Fiber();
	/**
	 * @brief 重置协程执行函数, 并设置状态
	 * @pre getState() 为INIT, TERM, EXCEPT
	 * @post getState() 为INIT
	 */
	void reset(std::function<void()> cb);
	void swapIn();
	void swapOut();

	// void call();
	// void back();

	uint64_t getId() const { return m_id; }

	State getState() const { return m_state; }

public:
	/**
	 * @brief 设置当前线程的运行协程
	 * @param f 运行协程
	 */
	static void SetThis(Fiber* f);

	/**
	 * @brief 返回当前所在协程
	 */
	static Fiber::ptr GetThis();

	/**
	 * @brief 将当前协程切换到后台, 并设置为READY状态
	 */
	static void YieldToReady();

	/**
	 * @brief 将当前协程切换到后台, 并设置为HOLD状态
	 */
	static void YieldToHold();

	/**
	 * @brief 返回当前协程总数
	 */
	static uint64_t TotalFibers();

	/**
	 * @brief 协程执行函数
	 * @post 执行完成返回到线程主协程
	 */
	static void MainFunc();

	/**
	 * @brief 协程执行函数
	 * @post 执行完成返回到线程调度协程
	 */
	// static void CallerMainFunc();

	/**
	 * @brief 获取当前协程id
	 */
	static uint64_t GetFiberId();

private:
	uint64_t m_id = 0;                  /// 协程id
	uint32_t m_stacksize = 0;           /// 协程栈大小
	State m_state = INIT;               /// 协程状态
	ucontext_t m_ctx;                   /// 协程上下文
	void* m_stack = nullptr;            /// 协程栈指针
	std::function<void()> m_cb;         /// 协程入口函数
	bool m_runInScheduler = false;      /// 该协程是否通过调度器来运行
};
} // namespace sylar

#endif //__FIBER_H__