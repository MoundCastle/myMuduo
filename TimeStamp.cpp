#include "TimeStamp.h"

#include <time.h>

TimeStamp::TimeStamp(/* args */) : microSecondsSinceEpoch(0) {};
TimeStamp::TimeStamp(time_t microSSE) : microSecondsSinceEpoch(microSSE) {};
TimeStamp TimeStamp::now()
{
    return TimeStamp(time(NULL));
}
std::string TimeStamp::toStr() const
{
    char buf[128];
    struct tm *ltm = localtime(&microSecondsSinceEpoch); // 注意这里传递的是地址
    snprintf(buf, 128, "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday,
             ltm->tm_hour,
             ltm->tm_min,
             ltm->tm_sec);
    return buf;
}

// int main()
// {
//     std::cout << TimeStamp::now().toStr() << std::endl;
//     return 0;
// }