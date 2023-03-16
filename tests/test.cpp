#include <iostream>
#include "../sylar/Log_System/Log.h"
#include "../sylar/Log_System/util.h"

int main(int argc, char** agrv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    
    logger->addAppender(sylar::LogAppender::ptr (new sylar::StdoutLogAppender));

    // sylar::LogEvent::ptr event(new sylar::LogEvent(logger, logger->getLevel() , __FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0), "123"));
    // event->getSS() << "Hello sylar log";

    // logger->log(sylar::LogLevel::DEBUG, event);
    // std::cout << "hello sylar log" << std::endl;

    SYLAR_LOG_DEBUG(logger); 

    auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(l) << "xxx";

}