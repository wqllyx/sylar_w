#pragma once
#include <fstream>
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>

namespace sylar_w
{
    // 日志事件
    class LogEvent
    {
    public:
        using ptr = std::shared_ptr<LogEvent>;

    public:
        LogEvent();

    private:
        const char *fileName_ = nullptr; // 文件名
        int32_t line_ = 0;

        uint32_t threadId_ = 0; // 线程ID

        uint32_t fiberId_ = 0; // 协程ID
        uint64_t time_;        // 时间戳
        uint32_t elapse;       // 程序启动到现在的毫秒数
        // 协程
        std::string content_;
    };

    // 日志级别
    class LogLevel
    {
    public:
        enum class Level
        {
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };
    };
    // 日志格式化器
    class LogFormatter
    {
    public:
        using ptr = std::shared_ptr<LogFormatter>;

    public:
        LogFormatter(const std::string &pattern);

        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream &format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    public:
        class FormatItem
        {
        public:
            using ptr = std::shared_ptr<FormatItem>;

        public:
            virtual ~FormatItem(){};
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        void init();
        bool isError() const { return error_; }
        const std::string getPattern() const { return pattern_; }

    private:
        std::string pattern_;
        std::vector<FormatItem::ptr> items_;
        bool error_ = false;
    };
} // namespace sylar_w
