#include "Log.h"

namespace sylar {
Logger::Logger(const std::string& name /* = "root" */)
    : m_name(name), m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        auto self = shared_from_this();
        if (!m_appenders.empty()) {
            for (auto& it : m_appenders) {
                // TODO:使用auto标明一个变量，这个变量永远不会是引用变量。
                it->log(self, level, event);
            }
        }
    }
}
void Logger::debug(LogEvent::ptr event) { log(LogLevel::DEBUG, event); }
void Logger::info(LogEvent::ptr event) { log(LogLevel::INFO, event); }
void Logger::warn(LogEvent::ptr event) { log(LogLevel::WARN, event); }
void Logger::error(LogEvent::ptr event) { log(LogLevel::ERROR, event); }
void Logger::fatal(LogEvent::ptr event) { log(LogLevel::FATAL, event); }
void Logger::addAppender(LogAppender::ptr appender) {
    if(!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender) {
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    // m_loggers[m_root->m_name] = m_root;

    // init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    // MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name);
    return it == m_loggers.end() ? m_root : it->second;
    // if(it != m_loggers.end()) {
    //     return it->second;
    // }

    // Logger::ptr logger(new Logger(name));
    // logger->m_root = m_root;
    // m_loggers[name] = logger;
    // return logger;
}

void LoggerManager::init() {
}

}