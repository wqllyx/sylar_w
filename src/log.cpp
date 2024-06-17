#include "log.h"
#include "util.h"
#include <map>
#include <functional>
#include <iostream>
#include <string>
#include <time.h>
namespace sylar_w
{

    const char *LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;

            XX(Level::DEBUG);
            XX(Level::INFO);
            XX(Level::WARN);
            XX(Level::ERROR);
            XX(Level::FATAL);
#undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
    }

    LogLevel::Level LogLevel::FromString(const std::string &str)
    {
#define XX(level, v)            \
    if (str == #v)              \
    {                           \
        return LogLevel::level; \
    }
        XX(Level::DEBUG, debug);
        XX(Level::INFO, info);
        XX(Level::WARN, warn);
        XX(Level::ERROR, error);
        XX(Level::FATAL, fatal);
        XX(Level::DEBUG, DEBUG);
        XX(Level::INFO, INFO);
        XX(Level::WARN, WARN);
        XX(Level::ERROR, ERROR);
        XX(Level::FATAL, FATAL);
        return Level::UNKNOW;
#undef XX
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string &thread_name)
        :fileName_(file), line_(line), elapse_(elapse), threadId_(thread_id)
        ,fiberId_(fiber_id), time_(time), threadName_(thread_name)
        ,logger_(logger), level_(level)
    {
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadName();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
            : format_(format)
        {
            if (format_.empty())
            {
                format_ = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), format_.c_str(), &tm);
            os << buf;
        }

    private:
        std::string format_;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string &str)
            : string_(str) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << string_;
        }

    private:
        std::string string_;
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << "\t";
        }

    private:
        std::string string_;
    };

    // Logger类
    Logger::Logger(const std::string &name)
        : name_(name), level_(LogLevel::Level::DEBUG)
    {
        formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        // 只有要求的日志级别大于日志对象时，才会记录。
        if (level >= level_)
        {
            auto self = shared_from_this();
            if (!appenders_.empty())
            {
                for (auto &i : appenders_)
                {
                    i->doLog(self, level, event);
                }
            }
            else if (root_)
            {
                root_->log(level, event);
            }
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
        // 如果当前appender的格式用户未定义，默认就用根logger的。
        if (!appender->getFormat())
        {
            appender->format_ = formatter_;
        }
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

    void Logger::clearAppenders()
    {
        appenders_.clear();
    }

    void Logger::setFormatter(LogFormatter::ptr val)
    {
        formatter_ = val;

        for (auto &i : appenders_)
        {
            if (!i->hasFormatter_)
            {
                i->format_ = formatter_;
            }
        }
    }

    void Logger::setFormatter(const std::string &val)
    {
        std::cout << "---" << val << std::endl;
        sylar_w::LogFormatter::ptr new_val(new sylar_w::LogFormatter(val));
        if (new_val->isError())
        {
            std::cout << "Logger setFormatter name=" << name_
                      << " value=" << val << " invalid formatter"
                      << std::endl;
            return;
        }
        // formatter_ = new_val;
        setFormatter(new_val);
    }

    LogFormatter::ptr Logger::getFormatter()
    {
        return formatter_;
    }

    void StdoutLogAppender::doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= getLevel())
        {
            format_->format(std::cout, logger, level, event);
        }
    }

    FileLogAppender::FileLogAppender(const std::string &filename)
        : filename_(filename)
    {
    }

    void FileLogAppender::doLog(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= level_)
        {
            // filestream_ << format_->format(logger, logger, level, event)
            if (!format_->format(filestream_, logger, level, event))
            {
                std::cout << "error" << std::endl;
            }
        }
    }
    bool FileLogAppender::reopen()
    {
        if (filestream_)
            filestream_.close();
        filestream_.open(filename_);
        return !!filestream_;
    }

    LogFormatter::LogFormatter(const std::string &pattern)
        : pattern_(pattern)
    {
        init();
    }

    std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : items_)
        {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    std::ostream &LogFormatter::format(std::ostream &ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        for (auto &i : items_)
        {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

    //%XXX %xxx{xxx} %%
    // "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
    void LogFormatter::init()
    {
        // 三元组：解析的格式字符串，实参，实参类型。
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr; // 存放无格式字符串（普通串） normalstring
        // 遍历模式字符串。
        for (size_t i = 0; i < pattern_.size(); ++i)
        {
            // 如果当前不是‘%’,说明这个不是模式，就是一个普通字符串，追加到临时nstr中
            if (pattern_[i] != '%')
            {
                nstr.append(1, pattern_[i]);
                continue;
            }
            // 如果如果是%%，说明是普通的%转义。追加，继续。
            if ((i + 1) < pattern_.size())
            {
                // 如果后面还是%，转义
                if (pattern_[i + 1] == '%')
                {
                    nstr.append(1, '%');
                    continue;
                }
            }

            // 确定是%xxx这种，还是%xxx{xxx}这种，此时当前i一定指向%，下一合法字符是`空字符`，`{`，`字母`其中之一。
            size_t n = i + 1;   // 下一个位置
            int fmt_status = 0; // 记录是否是%xxx{xxx}的flag：遇到`{`置位，遇到`}`复位。
            size_t fmt_begin = 0;
            std::string str; // 存放解析后的字符串
            std::string fmt; // 存放格式化字符串。

            // 循环，获取完整的下一参数。
            while (n < pattern_.size())
            {
                // 如果%后面既不是字母，也不是状态标识符‘{’和‘}’，那说明此参数已经解析完毕。break
                if (!fmt_status && (!isalpha(pattern_[n]) && pattern_[n] != '{' && pattern_[n] != '}'))
                {
                    // %d :比如这一个，i=0，n=2。 substr(1,1) = d
                    str = pattern_.substr(i + 1, n - i - 1);
                    break;
                }
                // 如果未遇到{，尝试遇到{
                if (fmt_status == 0)
                {
                    if (pattern_[n] == '{')
                    {
                        // 将前面的格式存起来，然后开始解析status：比如，%d{%Y，会把d存起来。
                        str = pattern_.substr(i + 1, n - i - 1);
                        // std::cout << "*" << str << std::endl;
                        fmt_status = 1; // 解析状态
                        fmt_begin = n;  // 此时fmt_begin相当于i的位置。
                        ++n;            // n就是从i的下一位置开始。
                        continue;
                    }
                }
                // 如果已经遇到过{，尝试遇到}，结束状态解析。
                else if (fmt_status == 1)
                {
                    if (pattern_[n] == '}')
                    {
                        fmt = pattern_.substr(fmt_begin + 1, n - fmt_begin - 1);
                        // std::cout << "#" << fmt << std::endl;
                        fmt_status = 0; // 复位
                        ++n;            // 继续解析下一个。
                        break;
                    }
                }
                // 否则，如果既不是{，也不是}，那一定是字母。如果未到结尾，只需继续循环即可。因为while第一步会存储格式str
                ++n; // 这里是%之后第2个字符了。因为已经确定%后面一个字符已经是字母了。

                // 如果%之后第2个字符已经到结尾，将最后一个格式保存，循环条件也因为不满足，不会继续进行了，
                if (n == pattern_.size())
                {
                    if (str.empty())
                    {
                        str = pattern_.substr(i + 1);
                    }
                }
            }

            // 正常情况
            if (fmt_status == 0)
            {
                // 如果格式不空，此串是普通字符串。
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                // 否则是格式字符串【比如：d】。状态是fmt【比如：{xxx}】
                vec.push_back(std::make_tuple(str, fmt, 1));
                // ??? todo
                i = n - 1;
            }
            // 正常不会出现1，出现说明{和}不配对。
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << pattern_ << " - " << pattern_.substr(i) << std::endl;
                error_ = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        // 最后一个串是无格式字符串的情况。
        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_items = {
#define XX(str, C) \
    {#str, [](const std::string &fmt) { return FormatItem::ptr(new C(fmt)); }}

            XX(m, MessageFormatItem),    // m:消息
            XX(p, LevelFormatItem),      // p:日志级别
            XX(r, ElapseFormatItem),     // r:累计毫秒数
            XX(c, NameFormatItem),       // c:日志名称
            XX(t, ThreadIdFormatItem),   // t:线程id
            XX(n, NewLineFormatItem),    // n:换行
            XX(d, DateTimeFormatItem),   // d:时间
            XX(f, FilenameFormatItem),   // f:文件名
            XX(l, LineFormatItem),       // l:行号
            XX(T, TabFormatItem),        // T:Tab
            XX(F, FiberIdFormatItem),    // F:协程id
            XX(N, ThreadNameFormatItem), // N:线程名称
#undef XX
        };

        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                items_.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    items_.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    error_ = true;
                }
                else
                {
                    items_.push_back(it->second(std::get<1>(i)));
                }
            }

            // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        // std::cout << items_.size() << std::endl;
    }
} // namespace sylar_w