# 笔记

### 1: 初始化列表中初始化 events_ 

```C++ 
#include <vector>  
#include <sys/epoll.h> // 假设您需要使用 epoll_event  
  
class MyClass {  
private:  
    using EventList = std::vector<epoll_event>;  
    EventList events_;  
  
public:  
    // 假设 kInitEventListSize 是一个已定义的常量  
    static constexpr size_t kInitEventListSize = 10; // 示例常量  
  
    // 构造函数  
    MyClass() : events_(kInitEventListSize) {} // 在初始化列表中初始化 events_  
  
    // 其他成员函数...  
};  
  
// 使用 MyClass  
MyClass myObject; // 这里，myObject 的 events_ 成员将被初始化为包含 kInitEventListSize 个 epoll_event 元素的 vector
```

在上面的代码中，`MyClass` 的构造函数没有参数，并且在其初始化列表中使用了 `events_(kInitEventListSize)` 来初始化 `events_` 成员变量。这里，`events_(kInitEventListSize)` 实际上是在调用 `std::vector<epoll_event>` 的构造函数，该构造函数接受一个大小参数并初始化 `vector` 以包含指定数量的默认构造的元素。

请注意，`kInitEventListSize` 必须是一个在编译时已知的常量表达式，或者至少是 `constexpr` 变量，以便它可以在编译时用于初始化列表。如果 `kInitEventListSize` 是一个运行时才能确定的变量，则您不能在初始化列表中直接使用它，而是需要在构造函数体内使用 `resize` 方法或其他方法来设置 `vector` 的大小。但是，在大多数情况下，使用初始化列表是更优雅且效率更高的方法。

### 2:  &\*events_.begin()

​	在C++中，`&*events_.begin()` 这种表达式通常用于获取 `events_`（一个 `std::vector<epoll_event>` 类型的成员变量）中第一个元素的地址。这里，`events_` 是一个 `std::vector` 容器，它存储了一系列 `epoll_event` 类型的对象。

让我们分解这个表达式来理解它是如何工作的：

1. **events_.begin()**：这是 `std::vector` 成员函数 `begin()` 的调用，它返回一个指向容器中第一个元素的迭代器。如果 `events_` 是空的，那么返回的迭代器将等于 `events_.end()`，但在这个上下文中，我们假设 `events_` 已经被初始化并且至少有一个元素，因为我们在讨论 `&*events_.begin()`。
2. ***events_.begin()**：迭代器解引用操作。`*` 操作符用于获取迭代器当前指向的元素的值。因此，`*events_.begin()` 实际上就是 `events_` 中第一个 `epoll_event` 对象的值。
3. **&\*events_.begin()**：现在，我们在解引用的元素（即 `events_` 中的第一个 `epoll_event` 对象）上应用 `&` 操作符。`&` 操作符用于获取其操作数的地址，因此 `&*events_.begin()` 返回的是 `events_` 中第一个 `epoll_event` 对象的内存地址。

这种表达式在需要直接访问容器中第一个元素的地址时非常有用。例如，在底层系统编程或与C语言接口交互时，可能需要直接操作内存地址。

然而，需要注意的是，如果 `events_` 是空的，则 `events_.begin()` 等于 `events_.end()`，此时解引用 `*events_.begin()` 是未定义行为（UB），因为它试图访问一个不存在的元素。因此，在使用这种表达式之前，最好先检查 `events_` 是否为空。

此外，`&*events_.begin()` 也可以写作 `&events_[0]`（如果 `events_` 不是空的），因为 `events_[0]` 访问的是 `events_` 中的第一个元素，而 `&events_[0]` 就是获取这个元素的地址。但是，如果 `events_` 是空的，`events_[0]` 也是未定义行为，所以同样需要先检查 `events_` 是否为空。





































