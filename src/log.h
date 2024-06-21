#pragma once
#include <fstream>
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <cstdarg>
#include "singleton.h"
#include "util.h"
/**
 * @brief 使用流式方式将日志级别level的日志写入到logger.这里为啥用LogEventWrap？因为流这个宏最终是要返回一个流对象的，但还需要同时调用Logger::log()，记录日志，无法完成，就是用了包装类。
 */
#define SYLAR_LOG_LEVEL(logger, level)                                                                                             \
    if (logger->getLevel() <= level)                                                                                               \
    sylar_w::LogEventWrap(sylar_w::LogEvent::ptr(new sylar::LogEvent(logger, level,                                                \
                                                                     __FILE__, __LINE__, 0, sylar_w::GetThreadId(),                \
                                                                     sylar_w::GetFiberId(), time(0), sylar_w::Thread::GetName()))) \
        .getSS()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar_w::LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别info的日志写入到logger
 */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar_w::LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar_w::LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar_w::LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar_w::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger 使用Logevent::format(const char *fmt, ...);
 */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                                                                                 \
    if (logger->getLevel() <= level)                                                                                                 \
    sylar_w::LogEventWrap(sylar_w::LogEvent::ptr(new sylar_w::LogEvent(logger, level,                                                \
                                                                       __FILE__, __LINE__, 0, sylar_w::GetThreadId(),                \
                                                                       sylar_w::GetFiberId(), time(0), sylar_w::Thread::GetName()))) \
        .getEvent()                                                                                                                  \
        ->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar_w::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar_w::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar_w::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar_w::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar_w::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获取主日志器
 */
#define SYLAR_LOG_ROOT() sylar_w::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获取name的日志器
 */
#define SYLAR_LOG_NAME(name) sylar_w::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar_w
{
    class Logger;
    class LoggerManager;

    /**
     * @brief 日志级别
     */
    class LogLevel
    {
    public:
        /**
         * @brief 日志级别枚举
         */
        enum class Level
        {
            /// 未知级别
            UNKNOW = 0,
            /// DEBUG 级别
            DEBUG = 1,
            /// INFO 级别
            INFO = 2,
            /// WARN 级别
            WARN = 3,
            /// ERROR 级别
            ERROR = 4,
            /// FATAL 级别
            FATAL = 5
        };

        /**
         * @brief 将日志级别转成文本输出
         * @param[in] level 日志级别
         */
        static const char *ToString(LogLevel::Level level);

        /**
         * @brief 将文本转换成日志级别
         * @param[in] str 日志级别文本
         */
        static LogLevel::Level FromString(const std::string &str);
    };

    /**
     * @brief 日志事件
     */
    class LogEvent
    {
    public:
        using ptr = std::shared_ptr<LogEvent>;

    public:
        /**
         * @brief 构造函数
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] file 文件名
         * @param[in] line 文件行号
         * @param[in] elapse 程序启动依赖的耗时(毫秒)
         * @param[in] thread_id 线程id
         * @param[in] fiber_id 协程id
         * @param[in] time 日志事件(秒)
         * @param[in] thread_name 线程名称
         */
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file,
                 int32_t line, uint32_t elapse, uint32_t thread_id,
                 uint32_t fiber_id, uint64_t time, const std::string &thread_name);
        /**
         * @brief 返回文件名
         */
        const char *getFile() const { return fileName_; }

        /**
         * @brief 返回行号
         */
        int32_t getLine() const { return line_; }

        /**
         * @brief 返回耗时
         */
        uint32_t getElapse() const { return elapse_; }

        /**
         * @brief 返回线程ID
         */
        uint32_t getThreadId() const { return threadId_; }

        /**
         * @brief 返回协程ID
         */
        uint32_t getFiberId() const { return fiberId_; }

        /**
         * @brief 返回时间
         */
        uint64_t getTime() const { return time_; }

        /**
         * @brief 返回线程名称
         */
        const std::string &getThreadName() const { return threadName_; }

        /**
         * @brief 返回日志内容
         */
        std::string getContent() const { return ss_.str(); }

        /**
         * @brief 返回日志器
         */
        std::shared_ptr<Logger> getLogger() const { return logger_; }

        /**
         * @brief 返回日志级别
         */
        LogLevel::Level getLevel() const { return level_; }

        /**
         * @brief 返回日志内容字符串流
         */
        std::stringstream &getSS() { return ss_; }

        // /**
        //  * @brief 格式化写入日志内容，给格式化宏使用的。
        //  */
        void format(const char *fmt, ...);

        // /**
        //  * @brief 格式化写入日志内容
        //  */
        void format(const char *fmt, va_list al);

    private:
        /// 文件名
        const char *fileName_ = nullptr;
        /// 行号
        int32_t line_ = 0;
        /// 程序启动开始到现在的毫秒数
        uint32_t elapse_ = 0;
        /// 线程ID
        uint32_t threadId_ = 0;
        /// 协程ID
        uint32_t fiberId_ = 0;
        /// 时间戳
        uint64_t time_ = 0;
        /// 线程名称
        std::string threadName_;
        /// 日志内容流
        std::stringstream ss_;
        /// 日志器
        std::shared_ptr<Logger> logger_;
        /// 日志等级
        LogLevel::Level level_;
    };

    /**
     * @brief 日志事件包装器
     */
    class LogEventWrap
    {
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
        LogEvent::ptr getEvent() const { return event_; }

        /**
         * @brief 获取日志内容流
         */
        std::stringstream &getSS();

    private:
        /**
         * @brief 日志事件
         */
        LogEvent::ptr event_;
    };

    // 日志格式化器
    class LogFormatter
    {
    public:
        using ptr = std::shared_ptr<LogFormatter>;

    public:
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
        LogFormatter(const std::string &pattern);

        /**
         * @brief 返回格式化日志文本
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream &format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    public:
        /**
         * @brief 日志内容项格式化
         */
        class FormatItem
        {
        public:
            using ptr = std::shared_ptr<FormatItem>;

        public:
            /**
             * @brief 析构函数
             */
            virtual ~FormatItem() {};

            /**
             * @brief 格式化日志到流
             * @param[in, out] os 日志输出流
             * @param[in] logger 日志器
             * @param[in] level 日志等级
             * @param[in] event 日志事件
             */
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        /**
         * @brief 初始化,解析日志模板
         */
        void init();

        /**
         * @brief 是否有错误
         */
        bool isError() const { return error_; }

        /**
         * @brief 返回日志模板
         */
        const std::string getPattern() const { return pattern_; }

    private:
        /// 日志格式模板
        std::string pattern_;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> items_;
        /// 是否有错误
        bool error_ = false;
    };

    /**
     * @brief 日志输出目标
     */
    class LoggerAppender
    {
        friend class Logger;

    public:
        using ptr = std::shared_ptr<LoggerAppender>;

    public:
        LoggerAppender() = default;
        /**
         * @brief 析构函数
         */
        virtual ~LoggerAppender() {};

        /**
         * @brief 写入日志
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        virtual void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormat() const { return format_; };
        /**
         * @brief 更改日志格式器
         */
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
        /// 日志级别
        LogLevel::Level level_;
        /// 是否有自己的日志格式器
        bool hasFormatter_ = false;
        /// 日志格式器
        LogFormatter::ptr format_;
    };

    /**
     * @brief 日志器
     */
    class Logger : public std::enable_shared_from_this<Logger>
    {
        friend class LoggerManager;
    public:
        using ptr = std::shared_ptr<Logger>;

    public:
        /**
         * @brief 构造函数
         * @param[in] name 日志器名称
         */
        Logger(const std::string &name = "root");

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
        void addAppender(LoggerAppender::ptr appender);

        /**
         * @brief 删除日志目标
         * @param[in] appender 日志目标
         */
        void delAppender(LoggerAppender::ptr appender);

        /**
         * @brief 清空日志目标
         */
        void clearAppenders();

        /**
         * @brief 返回日志级别
         */
        LogLevel::Level getLevel() const
        {
            return level_;
        }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val)
        {
            level_ = val;
        }

        /**
         * @brief 返回日志名称
         */
        const std::string &getName() const { return name_; }

        /**
         * @brief 设置日志格式器
         */
        void setFormatter(LogFormatter::ptr val);

        /**
         * @brief 设置日志格式模板
         */
        void setFormatter(const std::string &val);

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormatter();

    private:
        /// 日志名称
        std::string name_;
        // 日志级别
        LogLevel::Level level_;
        /// 日志格式器
        LogFormatter::ptr formatter_;
        /// 主日志器
        Logger::ptr root_;
        /// Appender集合
        std::list<LoggerAppender::ptr> appenders_;
    };

    /**
     *@brief 输出到控制台的Appender
     */
    class StdoutLogAppender : public LoggerAppender
    {
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;

    public:
        void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    /**
     * @brief 输出到文件的Appender
     */
    class FileLogAppender : public LoggerAppender
    {
    public:
        using ptr = std::shared_ptr<FileLogAppender>;

    public:
        FileLogAppender(const std::string &filename);
        void doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        /**
         * @brief 重新打开日志文件
         * @return 成功返回true
         */
        bool reopen();

    private:
        /// 文件路径
        std::string filename_;
        /// 文件流
        std::ofstream filestream_;
    };

    /**
     * @brief 日志器管理类
     */
    class LoggerManager
    {
    public:
        /**
         * @brief 构造函数
         */
        LoggerManager();

        /**
         * @brief 获取日志器
         * @param[in] name 日志器名称
         */
        Logger::ptr getLogger(const std::string &name);

        /**
         * @brief 初始化
         */
        void init();

        /**
         * @brief 返回主日志器
         */
        Logger::ptr getRoot() const { return root_; }

    private:
        /// 日志器容器
        std::map<std::string, Logger::ptr> loggers_;
        /// 主日志器
        Logger::ptr root_;
    };

    /// 日志器管理类单例模式
    typedef sylar::Singleton<LoggerManager> LoggerMgr;
} // namespace sylar_w
