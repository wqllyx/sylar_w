#include "log.h"
#include <map>
#include <functional>
namespace sylar_w
{
    Logger::Logger(const std::string &name)
        : name_(name)
    {
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        // 只有要求的日志级别大于日志对象时，才会记录。
        if (level_ >= level_)
        {
            // auto self = shared_from_this();
            // for (auto i : appenders_)
            // {
            //     i->doLog(self, level, event);
            // }
        }
    }
    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::Level::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::Level::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::Level::WARN, event);
    }
    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::Level::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::Level::FATAL, event);
    }

    void Logger::addAppender(LoggerAppender::ptr appender)
    {
        appenders_.push_back(appender);
    }
    void Logger::delAppender(LoggerAppender::ptr appender)
    {
        for (auto it = appenders_.begin(); it != appenders_.end(); ++it)
        {
            if (*it == appender)
            {
                appenders_.erase(it);
                break;
            }
        }
    }
} // namespace sylar_w