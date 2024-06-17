#include<iostream>
#include "log.h"
#include "util.h"
int main()
{
    sylar_w::Logger::ptr logger(new sylar_w::Logger());
    logger->addAppender(sylar_w::LoggerAppender::ptr(new sylar_w::StdoutLogAppender()));
    sylar_w::LogEvent::ptr event(new sylar_w::LogEvent(logger, sylar_w::LogLevel::Level::DEBUG,__FILE__, __LINE__,0, sylar_w::GetThreadId(), sylar_w::GetFiberId(),time(0),"线程名"));
    logger->log(sylar_w::LogLevel::Level::DEBUG, event);
    return 0;
}
