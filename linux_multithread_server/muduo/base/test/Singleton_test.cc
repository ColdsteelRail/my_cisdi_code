#include "muduo/base/Singleton.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Thread.h"

#include <stdio.h>

class Test : muduo::noncopyable
{
    public:
        Test()
        {
            printf("tid=%d, constructing %p\n", muduo::CurrentThread::tid(), this);
        }

        ~Test()
        {
            printf("tid=%d, destructing %p %s\n", muduo::CurrentThread::tid(), this, name_.c_str());
        }

        const muduo::string& name() const { return name_; }
        void setName(const muduo::string& n) { name_ = n; }

    private:
        muduo::string name_;
};

class TestNoDestroy : muduo::noncopyable
{
    public:
        // no_destroy未定义，因此不完整，不会析构
        void no_destroy();

        TestNoDestroy()
        {
            printf("tid=%d, constructing TestNoDestroy %p\n", muduo::CurrentThread::tid(), this);
        }

        ~TestNoDestroy()
        {
            printf("tid=%d, destructing TestNoDestroy %p\n", muduo::CurrentThread::tid(), this);
        }
};

void threadFunc()
{
    printf("tid=%d, %p name=%s\n",
         muduo::CurrentThread::tid(),
         &muduo::Singleton<Test>::instance(),
         muduo::Singleton<Test>::instance().name().c_str());

    muduo::Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
    // 主线程创建Test单例，打印tid=0, constructing 0x15a0040
    muduo::Singleton<Test>::instance().setName("only one");
    muduo::Thread t1(threadFunc);

    // 子线程调用threadFunc, 打印tid=1, 0x15a0040 name=only one
    // 设置name=only one, changed
    t1.start();
    t1.join();
    
    // 主线程打印tid=0, 0x15a0040 name=only one, changed
    printf("tid=%d, %p name=%s\n",
         muduo::CurrentThread::tid(),
         &muduo::Singleton<Test>::instance(),
         muduo::Singleton<Test>::instance().name().c_str());
    
    muduo::Singleton<TestNoDestroy>::instance();
    printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
// 主线程结束，Test对象销毁，调用析构函数
// 打印tid=0, destructing 0x15a0040 only one, changed

/*
tid=4156, constructing 0x25d7040
tid=4157, 0x25d7040 name=only one
tid=4156, 0x25d7040 name=only one, changed
tid=4156, constructing TestNoDestroy 0x25d7210
with valgrind, you should see 1-byte memory leak.
tid=4156, destructing 0x25d7040 only one, changed
*/