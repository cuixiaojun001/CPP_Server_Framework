#include <iostream>
#include "../sylar/log.h"

int main(int argc, char** agrv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    
    logger->addAppender(sylar::LogAppender::ptr (new sylar::StdoutLogAppender));

    sylar::LogEvent::ptr event(new sylar::LogEvent(logger, logger->getLevel() , __FILE__, __LINE__, 0, 1, 2, time(0), "123"));
    // event->getSS() << "Hello sylar log";

    logger->log(sylar::LogLevel::DEBUG, event);
    // std::cout << "hello sylar log" << std::endl;

}