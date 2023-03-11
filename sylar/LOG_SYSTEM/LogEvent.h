#ifndef __SYLAR_LOGEVENT_H__
#define __SYLAR_LOGEVENT_H__

#include "../packdef.h"

namespace sylar {
/**
 * @brief 日志事件
 */
class LogEvent {
   public:
    typedef std::shared_ptr<LogEvent> ptr;
    /**
     * @brief 构造函数
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] file 文件名
     * @param[in] line 文件行号
     * @param[in] elapse 线程启动依赖耗时(毫秒)
     * @param[in] thread_id 线程ID
     * @param[in] fiber_id 协程ID
     * @param[in] time 日志时间(秒)
     * @param[in] thread_name 线程名称
     */
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
             const char* file, int32_t line, uint32_t elapse,
             uint32_t thread_id, uint32_t fiber_id, uint64_t time,
             const std::string& threadname);


    /// @brief 返回文件名
    const char* getFile() const { return m_file; }
    /// @brief 返回行号
    int32_t getLine() const { return m_line; }
    /// @brief 返回耗时
    uint32_t getElapse() const { return m_elapse; }
    /// @brief 返回线程ID
    uint32_t getThreadId() const { return m_threadId; }
    /// @brief 返回协程ID
    uint32_t getFiberId() const { return m_fiberId; }
    /// @brief 返回时间
    uint64_t getTime() const { return m_time; }
    /// @brief 返回日志内容
    std::string getContent() const { return m_ss.str(); }
    /// @brief 返回日志器
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    /// @brief 返回日志级别
    LogLevel::Level getLevel() const { return m_level; }
    /// @brief 返回日志内容字符串流
    /// @todo & 和 const
    std::stringstream& getSS() { return m_ss; }
    /// @brief 返回线程名称
    const std::string& getThreadName() const { return m_threadName; }
    /// @brief 格式化写入日志内容
    void format(const char* fmt, ...);

   private:
    /// 日志内容流
    std::stringstream m_ss;
    /// 日志器
    std::shared_ptr<Logger> m_logger;
    /// 日志等级
    LogLevel::Level m_level;
    /// 文件名
    const char* m_file = nullptr;
    /// 行号
    int32_t m_line = 0;
    /// 程序启动开始到现在的毫秒数
    uint32_t m_elapse = 0;
    /// 线程ID
    uint32_t m_threadId = 0;
    /// 协程ID
    uint32_t m_fiberId = 0;
    /// 时间戳
    uint64_t m_time = 0;
    /// 线程名称
    std::string m_threadName;
};

/**
 * @brief 日志事件包装器
 */
class LogEventWrap {
    public:
    /**
     * @brief 构造函数
     * @param[in] e 日志事件
     */
    LogEventWrap(LogEvent::ptr e);

    /**
     * @brief 析构函数
     */
    ~LogEventWrap();

    /**
     * @brief 获取日志事件
     */
    LogEvent::ptr getEvent() const { return m_event;}

    /**
     * @brief 获取日志内容流
     */
    std::stringstream& getSS();

    private:
    /**
     * @brief 获取日志内容流
     */
    LogEvent::ptr m_event;
};
}

#endif