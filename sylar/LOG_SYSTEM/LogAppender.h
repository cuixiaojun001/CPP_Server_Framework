#ifndef __SYLAR_LOGAPPENDER_H__
#define __SYLAR_LOGAPPENDER_H__



namespace sylar {
class LogLevel;
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
    void setFormatter(std::shared_ptr<LogFormatter> val) { m_formatter = val; }

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
}  // namespace sylar

#endif