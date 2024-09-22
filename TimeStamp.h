#pragma once
#include <iostream>

class TimeStamp
{
private:
    time_t microSecondsSinceEpoch;

public:
    TimeStamp(/* args */);
    explicit TimeStamp(int64_t microSSE);
    static TimeStamp now();
    std::string toStr() const;
};
