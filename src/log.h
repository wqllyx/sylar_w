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
} // namespace sylar_w
