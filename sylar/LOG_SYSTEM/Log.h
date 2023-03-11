// 防御式声明
#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <stdint.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "singleton.h"

#define SYLAR_LOG_LEVEL(logger, level)                                  \
    if (logger->getLevel() <= level)                                    \
    sylar::LogEventWrap(                                                \
        sylar::LogEvent::ptr(new sylar::LogEvent(                       \
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
            sylar::GetFiberId(), time(0), "123")))                      \
        .getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

namespace sylar {
class Logger;

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
    LogEvent::ptr getEvent() const { return m_event; }

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
         * @param[in, out] os 日志输出流
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger,
                            LogLevel::Level level, LogEvent::ptr event) = 0;
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

/**
 * @brief 日志输出目标
 */
class LogAppender {
    // friend class Logger;
   public:
    typedef std::shared_ptr<LogAppender> ptr;
    /**
     * @brief 析构函数
     * @details
     * 如果基类的析构函数不是虚函数，当指向派生类的基类指针被回收时，只会执行基类的析构函数，容易造成内存泄漏
     */
    virtual ~LogAppender() {}

    /**
     * @brief 写入日志
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     * @details level, ptr是静态类型，非成员变量
     */
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                     LogEvent::ptr event) = 0;

    /**
     * @brief 更改日志器格式
     */
    void setFormatter(LogFormatter::ptr val) { m_formatter = val; }

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter() const { return m_formatter; }

    /**
     * @brief 获取日志级别
     */
    LogLevel::Level getLevel() const { return m_level; }

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val) { m_level = val; }

   protected:
    /// 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    /// 日志格式器
    LogFormatter::ptr m_formatter;
    /// 是否有自己的日志格式器
    bool m_hasFormatter = false;
};

/**
 * @brief 日志器
 */
class Logger : public std::enable_shared_from_this<Logger> {
    // friend class LogAppender;
   public:
    typedef std::shared_ptr<Logger> ptr;

    /**
     * @brief 构造函数
     * @param[in] name 日志器名称
     */

    Logger(const std::string& name = "root");

    /**
     * @brief 写日志
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /**
     * @brief 写debug级别日志
     * @param[in] event 日志事件
     */
    void debug(LogEvent::ptr event);

    /**
     * @brief 写info级别日志
     * @param[in] event 日志事件
     */
    void info(LogEvent::ptr event);

    /**
     * @brief 写warn级别日志
     * @param[in] event 日志事件
     */
    void warn(LogEvent::ptr event);

    /**
     * @brief 写error级别日志
     * @param[in] event 日志事件
     */
    void error(LogEvent::ptr event);

    /**
     * @brief 写fatal级别日志
     * @param[in] event 日志事件
     */
    void fatal(LogEvent::ptr event);

    /**
     * @brief 添加日志目标
     * @param[in] appender 日志目标
     */
    void addAppender(LogAppender::ptr appender);

    /**
     * @brief 删除日志目标
     * @param[in] appender 日志目标
     */
    void delAppender(LogAppender::ptr appender);

    /**
     * @brief 清空日志目标
     */
    void clearAppenders();

    /**
     * @brief 返回日志级别
     */
    LogLevel::Level getLevel() const { return m_level; }

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val) { m_level = val; }

    /**
     * @brief 返回日志名称
     */
    const std::string& getName() const { return m_name; }

   private:
    /// 日志名称
    std::string m_name;
    /// 日志级别
    LogLevel::Level m_level;
    /// 日志目标集合
    std::list<LogAppender::ptr> m_appenders;
    /// 日志格式器
    LogFormatter::ptr m_formatter;
};

/**
 * @brief 输出到控制台的Appender
 */
class StdoutLogAppender : public LogAppender {
   public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
             LogEvent::ptr event) override;
};

/**
 * @brief 输出到文件的Appender
 */
class FileLogAppender : public LogAppender {
   public:
    FileLogAppender(const std::string& filename);
    typedef std::shared_ptr<FileLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
             LogEvent::ptr event) override;

    /**
     * @brief 重新打开日志文件
     * @return 成功返回true
     */
    bool reopen();

   private:
    /// 文件路径
    std::string m_filename;
    /// 文件流
    std::ofstream m_filestream;
};

class MessageFormatItem : public LogFormatter::FormatItem {
   public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger,
                LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
   public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger,
                LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
   public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger,
                LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
   public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
   public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
   public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
   public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
   public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        : m_format(format) {
        if (m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }

   private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
   public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
   public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
   public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
   public:
    StringFormatItem(const std::string& str) : m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << m_string;
    }

   private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
   public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event) override {
        os << "\t";
    }

   private:
    std::string m_string;
};

/**
 * @brief 日志器管理类
 */
class LoggerManager {
   public:
    // typedef Spinlock MutexType;
    /**
     * @brief 构造函数
     */
    LoggerManager();

    /**
     * @brief 获取日志器
     * @param[in] name 日志器名称
     */
    Logger::ptr getLogger(const std::string& name);

    /**
     * @brief 初始化
     */
    void init();

    /**
     * @brief 返回主日志器
     */
    Logger::ptr getRoot() const { return m_root; }

    /**
     * @brief 将所有的日志器配置转成YAML String
     */
    // std::string toYamlString();
   private:
    /// Mutex
    // MutexType m_mutex;
    /// 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    /// 主日志器
    Logger::ptr m_root;
};

/// 日志器管理类单例模式
typedef sylar::Singleton<LoggerManager> LoggerMgr;

}  // namespace sylar

#endif