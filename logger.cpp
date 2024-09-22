#include "logger.h"
#include "TimeStamp.h"

// 获得单例
Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}
// 日志级别
void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}
// 打印日志
void Logger::log(std::string msg)
{
    switch (logLevel_)
    {
    case DEBUG:
        std::cout << "[DEBUG]\t";
        break;
    case INFO:
        std::cout << "[INFO]\t";
        break;
    case WARN:
        std::cout << "[WARN]\t";
        break;
    case ERROR:
        std::cout << "[ERROR]\t";
        break;
    case FATAL:
        std::cout << "[FATAL]\t";
        break;
    default:
        break;
    }

    std::cout << TimeStamp::now().toStr() << " : " << msg << std::endl;
}

// int main()
// {
//     int N = 0;
//     while (N++ < 10)
//     {
//         LOG_DEBUG("debug %d", N);
//         LOG_INFO("INFO %d", N);
//         LOG_WARN("WARN %d", N);
//         LOG_ERROR("ERROR %d", N);
//     }
//     return 0;
// }