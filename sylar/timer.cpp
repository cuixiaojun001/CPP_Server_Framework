#include "timer.h"
#include "util.h"

namespace sylar {
bool Timer::Comparator::operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const {
	if (!lhs && !rhs)
		return false;
	if (!lhs)
		return true;
	if (!rhs)
		return false;
	if (lhs->m_next < rhs->m_next)
		return true;
	if (lhs->m_next > rhs->m_next)
		return false;
	return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimeManager* manager) 
    : m_ms(ms)
    , m_recurring(recurring)
    , m_cb(cb)
    , m_manager(manager) {
	m_next = sylar::GetCurrentMs() + m_ms;
}
Timer::Timer(uint64_t next) : m_next(next) {
}

bool Timer::cancel() {
	TimeManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (m_cb) {
		m_cb = nullptr;
		auto it = m_manager->m_timers.find(shared_from_this());
		m_manager->m_timers.erase(it);
		return true;
	}
	return false;
}
bool Timer::refresh() {
	TimeManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {    // 如果不存在回调函数, 刷新失败
		return false;
	}
	auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
		return false;
	}
	m_manager->m_timers.erase(it);
	m_next = sylar::GetCurrentMs() + m_ms;
	m_manager->m_timers.insert(shared_from_this());
	return true;
}
bool Timer::reset(uint64_t ms, bool from_now) {
    if (ms == m_ms && !from_now) {
		return true;
	}
	TimeManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
		return false;
	}
	auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
		return false;
	}
	m_manager->m_timers.erase(it);
	uint64_t start = 0;
    if (from_now) {
		start = sylar::GetCurrentMs();
	} else {
		start = m_next - m_ms;
	}
	m_ms = ms;
	m_next = start + ms;
	m_manager->addTimer(shared_from_this(), lock);
	return true;
}

TimeManager::TimeManager() {
	m_previouseTime = GetCurrentMs();
}
TimeManager::~TimeManager() {

}

Timer::ptr TimeManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
	Timer::ptr timer(new Timer(ms, cb, recurring, this));
	RWMutex::WriteLock lock(m_mutex);
	addTimer(timer, lock);
	return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::ptr TimeManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring) {
	return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimeManager::getNextTimer() {
	RWMutexType::ReadLock lock(m_mutex);
	m_tickled = false;
    if (m_timers.empty()) {
		return ~0ull; // 2^64 - 1
	}
	const Timer::ptr& next = *m_timers.begin();
	uint64_t now_ms = GetCurrentMs();
    if (now_ms >= next->m_next) {
		return 0;
	} else {
		return next->m_next - now_ms;
	}
}

void TimeManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
	uint64_t now_ms = sylar::GetCurrentMs();
	std::vector<Timer::ptr> expired;
    {
		RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) 
			return;
	}
	RWMutexType::WriteLock lock(m_mutex);
    if (m_timers.empty()) {
		return;
	}
	bool rollover = detectClockRollover(now_ms); // 检测服务器时间是否被调后了
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) // 
		return;
	Timer::ptr now_timer(new Timer(now_ms)); // 
	auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
	while (it != m_timers.end() && (*it)->m_next == now_ms) ++it;
	expired.insert(expired.begin(), m_timers.begin(), it);
	m_timers.erase(m_timers.begin(), it);
	cbs.reserve(expired.size());
    for (auto& timer: expired) {
		cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
			timer->m_next = now_ms + timer->m_ms;
			m_timers.insert(timer);
		} else {
			timer->m_cb = nullptr;
		}
	}
}

bool TimeManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

void TimeManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock) {
	auto it = m_timers.insert(val).first; // 返回由指向被插入元素（或阻止插入的元素）的迭代器
	bool at_front = (it == m_timers.begin()) && !m_tickled; // 插入的定时器是超时绝对时间最近
    if(at_front) {
		m_tickled = true;
	}
	lock.unlock();
    if (at_front) {
		onTimerInsertedAtFront();
	}
}

bool TimeManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if(now_ms < m_previouseTime &&
            now_ms < (m_previouseTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}
} // namespace sylar
