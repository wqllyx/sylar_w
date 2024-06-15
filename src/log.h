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

    // 日志输出地
    class LoggerAppender
    {
    public:
        using ptr = std::shared_ptr<LoggerAppender>;

    public:
        LoggerAppender() = default;
        virtual ~LoggerAppender();
        virtual void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        LogFormatter::ptr getFormat() const { return format_; };
        void setFormat(LogFormatter::ptr format) { format_ = format_; }
        /**
         * @brief 获取日志级别
         */
        LogLevel::Level getLevel() const { return level_; }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val) { level_ = val; }

    protected:
        LogLevel::Level level_;
        LogFormatter::ptr format_;
    };
    // 日志器
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;

    public:
        Logger(const std::string &name = "root");

        /* 调用此函数记录日志：
        参数：
            level :要记录的日志级别。
            event: 要记录的日志事件。
             */
        void log(LogLevel::Level level, LogEvent::ptr event);
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LoggerAppender::ptr appender);
        void delAppender(LoggerAppender::ptr appender);
        LogLevel::Level getLevel() const
        {
            return level_;
        }
        void setLevel(LogLevel::Level val)
        {
            level_ = val;
        }

    private:
        std::string name_;                         // 日志名称
        LogLevel::Level level_;                    // 日志级别
        std::list<LoggerAppender::ptr> appenders_; // Appender集合
    };

    // 输出到控制台
    class StdoutLogAppender : public LoggerAppender
    {
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;

    public:
        void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    // 输出到文件
    class FileLogAppender : public LoggerAppender
    {
    public:
        using ptr = std::shared_ptr<FileLogAppender>;

    public:
        FileLogAppender(const std::string &filename);
        void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        bool reopen();

    private:
        std::string filename_;
        std::ofstream filestream_;
    };
} // namespace sylar_w
