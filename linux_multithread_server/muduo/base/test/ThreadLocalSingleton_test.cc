#include "muduo/base/ThreadLocalSingleton.h"
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

void threadFunc(const char* changeTo)
{
  printf("tid=%d, %p name=%s\n",
         muduo::CurrentThread::tid(),
         &muduo::ThreadLocalSingleton<Test>::instance(),
         muduo::ThreadLocalSingleton<Test>::instance().name().c_str());
  muduo::ThreadLocalSingleton<Test>::instance().setName(changeTo);
  printf("tid=%d, %p name=%s\n",
         muduo::CurrentThread::tid(),
         &muduo::ThreadLocalSingleton<Test>::instance(),
         muduo::ThreadLocalSingleton<Test>::instance().name().c_str());

  // no need to manually delete it
  // muduo::ThreadLocalSingleton<Test>::destroy();
}

int main()
{
  muduo::ThreadLocalSingleton<Test>::instance().setName("main one"); // line 61
  muduo::Thread t1(std::bind(threadFunc, "thread1"));
  muduo::Thread t2(std::bind(threadFunc, "thread2"));
  t1.start(); // line 62~64
  t2.start(); // line 67~69
  t1.join();  // line 65
  printf("tid=%d, %p name=%s\n",
         muduo::CurrentThread::tid(),
         &muduo::ThreadLocalSingleton<Test>::instance(),
         muduo::ThreadLocalSingleton<Test>::instance().name().c_str()); // line 66
  t2.join(); // line 70

  pthread_exit(0);  // line 71
}

/*
tid=4084, constructing 0x157b040
tid=4085, constructing 0x7f73240008c0
tid=4085, 0x7f73240008c0 name=
tid=4085, 0x7f73240008c0 name=thread1
tid=4085, destructing 0x7f73240008c0 thread1
tid=4084, 0x157b040 name=main one
tid=4086, constructing 0x7f73240008c0
tid=4086, 0x7f73240008c0 name=
tid=4086, 0x7f73240008c0 name=thread2
tid=4086, destructing 0x7f73240008c0 thread2
tid=4084, destructing 0x157b040 main one
*/