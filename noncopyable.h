#pragma once

class noncopyable
{
private:
    noncopyable(noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

public:
    ~noncopyable() = default;
    noncopyable(/* args */) = default;
};
