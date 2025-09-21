// Logger.hpp
#pragma once

#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <fstream>

// 日志级别
enum class LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 日志输出策略接口
class LogStrategy {
public:
    virtual ~LogStrategy() = default;
    virtual void log(const std::string& msg) = 0;
};

// 控制台输出策略
class ConsoleLogStrategy : public LogStrategy {
public:
    void log(const std::string& msg) override {
        std::cout << msg << std::endl;
    }
};

// 文件输出策略
class FileLogStrategy : public LogStrategy {
    std::ofstream _file;
public:
    FileLogStrategy(const std::string& filename) {
        _file.open(filename, std::ios::app);
        if (!_file.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }
    ~FileLogStrategy() {
        if (_file.is_open()) _file.close();
    }
    void log(const std::string& msg) override {
        if (_file.is_open()) {
            _file << msg << std::endl;
            _file.flush(); // 立即写入磁盘
        }
    }
};

class LogStream {
public:
    LogStream(Logger& logger, LogLevel level, const char* file, int line)
        : _logger(logger), _level(level), _file(file), _line(line) {}

    template <typename T>
    LogStream& operator<<(const T& value) {
        std::ostringstream oss;
        oss << value;
        _logger.log(_level, _file, _line, "%s", oss.str().c_str());
        return *this;
    }

private:
    Logger& _logger;
    LogLevel _level;
    const char* _file;
    int _line;
};

// 全局日志器（单例 + 线程安全）
class Logger {
private:
    LogStrategy* _strategy;
    std::mutex _mtx;

    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::FATAL:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }

public:
    Logger() : _strategy(nullptr) {}

    void setStrategy(LogStrategy* strategy) {
        std::lock_guard<std::mutex> lock(_mtx);
        _strategy = strategy;
    }

    void log(LogLevel level, const char* file, int line, const char* format, ...) {
        if (!_strategy) return;

        char time_buf[64];
        time_t now = time(nullptr);
        tm* tm_now = localtime(&now);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_now);

        char buf[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        char final_msg[2048];
        snprintf(final_msg, sizeof(final_msg),
                 "[%s] [%s:%d] %s",
                 levelToString(level).c_str(),
                 file, line, buf);

        std::lock_guard<std::mutex> lock(_mtx);
        _strategy->log(std::string(final_msg));
    }
};

// 全局日志器实例
static Logger g_logger;

// 设置日志策略
inline void EnableConsoleLogStrategy() {
    g_logger.setStrategy(new ConsoleLogStrategy());
}

inline void EnableFileLogStrategy(const std::string& filename) {
    g_logger.setStrategy(new FileLogStrategy(filename));
}

// 日志宏（自动传入文件名、行号）
#define LOG(level) \
    if (true) { \
        LogStream(g_logger, level, __FILE__, __LINE__); \
    } else (void)0

// 带格式化参数的日志宏
#define LOG_FORMAT(level, fmt, ...) g_logger.log(level, __FILE__, __LINE__, fmt, __VA_ARGS__)
