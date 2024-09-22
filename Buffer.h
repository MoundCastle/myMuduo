#pragma once
#include "noncopyable.h"

#include <string>
#include <vector>
#include <algorithm>

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |        8           |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend)
    {
    }
    // 可读取数据的长度
    size_t readableBytes() const
    {
        return writerIndex_ - readerIndex_;
    }

    // 可写数据长度
    size_t writableBytes() const
    {
        return buffer_.size() - writerIndex_;
    }

    // 头部空闲结束位置
    size_t prependableBytes() const
    {
        return readerIndex_;
    }

    const char *peek() const
    {
        return begin() + readerIndex_;
    }
    // 取出 len 位数据， 并修改readindex_
    void retrieve(size_t len)
    {
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
    }
    // data 开始 len 长度 数据  添加到缓冲区
    void append(const char * /*restrict*/ data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }
    char *beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char *beginWrite() const
    {
        return begin() + writerIndex_;
    }

    ssize_t readFd(int fd, int *savedErrno);  // 从 来源 fd 写入 缓冲区
    ssize_t writeFd(int fd, int *savedErrno); // 从缓冲区 发送 到相应的 目的fd

private:
    char *begin()
    {
        return &*buffer_.begin(); // 起始地址
    }

    const char *begin() const
    {
        return &*buffer_.begin();
    }

    void makeSpace(size_t len)
    {
        // 可写的位数 和 头部空闲 是否满足 len
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // vector 扩容
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            // 移动当前 buffer 集中空间
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};
