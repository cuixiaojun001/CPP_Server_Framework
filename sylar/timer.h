/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-07 21:42:20
 * @LastEditTime: 2023-05-07 23:14:30
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include "mutex.h"
#include <functional>
#include <memory>
#include <set>
#include <vector>

namespace sylar {
class TimeManager;
class Timer : public std::enable_shared_from_this<Timer> {
	friend class TimeManager;

public:
	typedef std::shared_ptr<Timer> ptr;
	/**
	 * @brief 取消定时器
	 */
	bool cancel();
	/**
	 * @brief 刷新设置定时器的执行时间
	 */
	bool refresh();
	/**
	 * @brief 重置定时器时间
	 * @param ms 超时时间
	 * @param from_now 是否从当前时间开始计算
	 */
	bool reset(uint64_t ms, bool from_now);

private:
	/**
	 * @brief 私有构造, 只能通过TimeManager来创建Timer对象
	 * @param[in] ms 定时器超时时间
	 * @param[in] cb 回调函数
	 * @param[in] recurring 是否循环
	 * @param[in] manager 定时器管理器
	 */
	Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimeManager* manager);
	/**
	 * @brief 私有构造
	 * @param next 执行的绝对时间戳(毫秒)
	 */
	Timer(uint64_t next);

private:
	/// 精确的执行时间
	uint64_t m_next = 0;
	/// 超时周期
	uint64_t m_ms = 0;
	/// 精确的执行时间
	bool m_recurring = false;
	/// 回调函数
	std::function<void()> m_cb;
	/// 定时器管理器
	TimeManager* m_manager = nullptr;

private:
	struct Comparator {
		/**
		 * @brief 比较定时器的智能指针的大小(按执行时间排序)
		 * @param lhs
		 * @param rhs
		 */
		bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
	};
};


class TimeManager {
	friend class Timer;

public:
	/// 读写锁类型
	typedef RWMutex RWMutexType;
	/**
	 * @brief 构造函数
	 */
	TimeManager();
	/**
	 * @brief 析构函数
	 */
	virtual ~TimeManager();
	/**
	 * @brief 添加定时器
	 * @param[in] ms 定时器执行间隔时间
	 * @param[in] cb 定时器回调函数
	 * @param[in] recurring 是否循环定时器
	 */
	Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
	/**
	 * @brief 添加条件定时器
	 * @param[in] ms 定时器执行间隔时间
	 * @param[in] cb 定时器回调函数
	 * @param[in] weak_cond 条件
	 * @param[in] recurring 是否循环
	 */
	Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);
	
    /**
	 * @brief 到最近一个定时器执行的时间间隔(毫秒)
	 */
	uint64_t getNextTimer();
    
	/**
	 * @brief 获取需要执行的定时器的回调函数列表
	 * @param[out] cbs 回调函数数组
	 */
	void listExpiredCb(std::vector<std::function<void()> >& cbs);
	/**
	 * @brief 是否有定时器
	 */
	bool hasTimer();

protected:
	/**
	 * @brief 当有新的定时器插入到定时器的首部,执行该函数
	 */
	virtual void onTimerInsertedAtFront() = 0;

	/**
	 * @brief 将定时器添加到管理器中
	 */
	void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

private:
	/**
	 * @brief 检测服务器时间是否被调后了
	 */
	bool detectClockRollover(uint64_t now_ms);

private:
	/// Mutex
	RWMutexType m_mutex;
	/// 定时器集合
	std::set<Timer::ptr, Timer::Comparator> m_timers;
	/// 是否触发onTimerInsertedAtFront
	bool m_tickled = false;
	/// 上次执行时间
	uint64_t m_previouseTime = 0;
};
} // namespace sylar

#endif //__TIMER_H__