#ifndef __SYLAR_LOGLEVEL_H__
#define __SYLAR_LOGLEVEL_H__

#include <string>

namespace sylar {
class LogLevel;
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
}

#endif