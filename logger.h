#pragma once

#include <string>

#include "noncopyable.h"

// 日志等级： trace Debug Info WARN Error Fatal

#ifndef MUDEBUG
#define LOG_DEBUG(LogMsg, ...)                      \
    do                                              \
    {                                               \
        Logger &logger = Logger::instance();        \
        logger.setLogLevel(DEBUG);                  \
        char buf[1024];                             \
        snprintf(buf, 1024, LogMsg, ##__VA_ARGS__); \
        logger.log(buf);                            \
    } while (0);
#else
#define LOG_DEBUG(LogMsg, ...)
#endif

#define LOG_INFO(LogMsg, ...)                       \
    do                                              \
    {                                               \
        Logger &logger = Logger::instance();        \
        logger.setLogLevel(INFO);                   \
        char buf[1024];                             \
        snprintf(buf, 1024, LogMsg, ##__VA_ARGS__); \
        logger.log(buf);                            \
    } while (0);

#define LOG_WARN(LogMsg, ...)                       \
    do                                              \
    {                                               \
        Logger &logger = Logger::instance();        \
        logger.setLogLevel(WARN);                   \
        char buf[1024];                             \
        snprintf(buf, 1024, LogMsg, ##__VA_ARGS__); \
        logger.log(buf);                            \
    } while (0);

#define LOG_ERROR(LogMsg, ...)                      \
    do                                              \
    {                                               \
        Logger &logger = Logger::instance();        \
        logger.setLogLevel(ERROR);                  \
        char buf[1024];                             \
        snprintf(buf, 1024, LogMsg, ##__VA_ARGS__); \
        logger.log(buf);                            \
    } while (0);

#define LOG_FATAL(LogMsg, ...)                      \
    do                                              \
    {                                               \
        Logger &logger = Logger::instance();        \
        logger.setLogLevel(FATAL);                  \
        char buf[1024];                             \
        snprintf(buf, 1024, LogMsg, ##__VA_ARGS__); \
        logger.log(buf);                            \
        exit(-1);                                   \
    } while (0);

enum LogLevel
{
    DEBUG, // 调试信息
    INFO,  // 信息级别
    WARN,  // 警告级别
    ERROR,
    FATAL, // 记录严重的错误，这些错误会导致程序崩溃或无法继续运行
};

class Logger : noncopyable
{
public:
    static Logger &instance();
    void setLogLevel(int level);
    void log(std::string msg);

private:
    int logLevel_;
    Logger() {};
};