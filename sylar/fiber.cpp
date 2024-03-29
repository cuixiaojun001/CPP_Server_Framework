/*
 * @Author: Cui XiaoJun
 * @Date: 2023-04-30 21:13:02
 * @LastEditTime: 2023-05-09 13:59:20
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "fiber.h"
#include "log.h"
#include "config.h"
#include "macro.h"
#include "scheduler.h"

namespace sylar {
/// 全局静态变量: 生成协程id
static std::atomic<uint64_t> s_fiber_id{0};
/// 全局静态变量: 统计当前的协程数
static std::atomic<uint64_t> s_fiber_count{0};
/// 线程局部变量: 当前线程正在运行的协程
static thread_local Fiber* t_fiber = nullptr;
/// 线程局部变量: 当前线程的主协程, 切换到这个协程, 就相当于切换到了主线程中运行
static thread_local Fiber::ptr t_thread_fiber = nullptr;

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }
    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};
using StackAllocator = MallocStackAllocator;

/*类成员方法*/

/*----------------构造方法----------------*/
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);
    if (getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber() 创建主协程" << " total=" << s_fiber_count;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler)
    : m_id(++s_fiber_id), m_cb(cb), m_runInScheduler(run_in_scheduler) {
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    // if(!use_caller) {
    makecontext(&m_ctx, &Fiber::MainFunc, 0); 
    // } else {
        // makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    // }
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber(arg) id=" << m_id << " total=" << s_fiber_count;
}
/*----------------析构函数----------------*/
Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        SYLAR_ASSERT1(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        SYLAR_ASSERT1(!m_cb);
        SYLAR_ASSERT1(m_state == EXEC);

        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total=" << s_fiber_count;
}

/*----------------静态方法----------------*/
Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        // SYLAR_LOG_DEBUG(g_logger) << "Current fiber id: " << t_fiber->getId(); 
        return t_fiber->shared_from_this();
    }
    /* 如果当前线程还未创建协程, 则创建线程的第一个协程 */
    Fiber::ptr main_fiber(new Fiber);
    // SYLAR_LOG_DEBUG(g_logger) << "Creating main fiber for thread: " << main_fiber->getId();
    SYLAR_ASSERT1(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    
    // 返回一个shared_ptr指针, 该指针指向调用他的对象
    return t_fiber->shared_from_this(); 
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
	SYLAR_ASSERT1(cur->m_state == EXEC);
	// cur->m_state = HOLD;
	cur->swapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
	SYLAR_ASSERT1(cur);
	try
    {
        cur->m_cb();    
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch(const std::exception& e)
    {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:" << e.what();
    } catch(...) {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:";
    }
    auto raw_ptr = cur.get(); // 得到裸指针
    cur.reset(); // 释放智能指针
    raw_ptr->swapOut();
    
    SYLAR_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

/*----------------普通类成员函数----------------*/

void Fiber::reset(std::function<void()> cb) {
    SYLAR_ASSERT1(m_stack);
    SYLAR_ASSERT1(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}   

void Fiber::swapIn() {
    SetThis(this);
    SYLAR_ASSERT1(m_state != EXEC);
	m_state = EXEC;
	if (m_runInScheduler) {
        if (swapcontext(&(Scheduler::GetMainFiber()->m_ctx), &m_ctx)) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    } else {
        if (swapcontext(&t_thread_fiber->m_ctx, &m_ctx)) {
            SYLAR_ASSERT2(false, "swapcontext");
	    }
    }
	
}
void Fiber::swapOut() {
	SetThis(t_thread_fiber.get());
	if (m_runInScheduler) {
        if (swapcontext(&m_ctx, &(Scheduler::GetMainFiber()->m_ctx))) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    } else {
		SYLAR_LOG_DEBUG(g_logger) << "主协程id: " << t_thread_fiber->m_id << " 主协程上下文: " << &t_thread_fiber->m_ctx;
		if (swapcontext(&m_ctx, &t_thread_fiber->m_ctx)) {
			SYLAR_ASSERT2(false, "swapcontext");
		}
	}
}

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}   

}