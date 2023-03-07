// 防御式声明
// #ifdef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <functional>

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
     */
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
             const char* file, int32_t line, uint32_t elapse,
             uint32_t thread_id, uint32_t fiber_id, uint64_t time);

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
    /// @brief 格式化写入日志内容
    void format(const char* fmt, ...);

   private:
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
    /// 日志内容流
    std::stringstream m_ss;
    /// 日志器
    std::shared_ptr<Logger> m_logger;
    /// 日志等级
    LogLevel::Level m_level;
};

/**
 * @brief 日志级别
 */
class LogLevel {
   public:
    /**
     * @brief 日志级别枚举
     */
    enum Level {
        /// 未知级别
        UNKNOW = 0,
        /// DEBUG级别
        DEBUG = 1,
        /// INFO级别
        INFO = 2,
        /// WARN级别
        WARN = 3,
        /// ERROR级别
        ERROR = 4,
        /// FATAL级别
        FATAL = 5
    };

    /**
     * @brief 将日志级别转换成文本输出
     * @param[in] level 日志级别
     */
    static const char* ToString(LogLevel::Level level);

    /**
     * @brief 将文本转换成日志级别
     * @param[in] str 日志级别文本
     */
    static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志格式器
 */
class LogFormatter {
   public:
    typedef std::shared_ptr<LogFormatter> ptr;
    /**
     * @brief 构造函数
     * @param[in] pattern 格式模板
     * @details
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern);

    /**
     * @brief 返回格式化日志文本
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,
                       LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger,
                         LogLevel::Level level, LogEvent::ptr event);

   public:
    /// @brief 日志内容项格式化
    class FormatItem {
       public:
        typedef std::shared_ptr<FormatItem> ptr;
        /// @brief 析构函数
        virtual ~FormatItem() {}
        /**
         * @brief 格式化日志到流
         * @param os 日志输出流
         * @param level 日志级别
         * @param event 日志事件
         */
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                            LogEvent::ptr event) = 0;
    };
    /**
     * @brief 初始化,解析日志模板
     */
    void init();

    /**
     * @brief 是否有错误
     */
    bool isError() const { return m_error; }

    /**
     * @brief 返回日志模板
     */
    const std::string getPattern() const { return m_pattern; }

   private:
    /// 日志模板格式
    std::string m_pattern;
    /// 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
    /// 是否有错误
    bool m_error = false;
};

// 日志输出源
class LogAppender {
   public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {
    }  // 如果基类的析构函数不是虚函数，当指向派生类的基类指针被回收时，只会执行基类的析构函数，容易造成内存泄漏
    virtual void log(
        Logger::ptr logger, LogLevel::Level level,
        LogEvent::ptr event) = 0;  // level, ptr是静态类型，非成员变量
    void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }

   protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger> {
   public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level val) { m_level = val; }

   private:
    std::string m_name;                       // 日志名称
    LogLevel::Level m_level;                  // 日志级别
    std::list<LogAppender::ptr> m_appenders;  // Appender集合
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
   public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level,
             LogEvent::ptr event) override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender {
   public:
    FileLogAppender(const std::string& filename);
    typedef std::shared_ptr<FileLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level,
             LogEvent::ptr event) override;

    bool reopen();

   private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}  // namespace sylar

// #endif