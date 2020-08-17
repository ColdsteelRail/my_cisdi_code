#include "muduo/base/ThreadLocal.h"
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

muduo::ThreadLocal<Test> testObj1;
muduo::ThreadLocal<Test> testObj2;

void print()
{
  printf("tid=%d, obj1 %p name=%s\n",
         muduo::CurrentThread::tid(),
         &testObj1.value(),
         testObj1.value().name().c_str());
  printf("tid=%d, obj2 %p name=%s\n",
         muduo::CurrentThread::tid(),
         &testObj2.value(),
         testObj2.value().name().c_str());
}

void threadFunc()
{
  print();
  testObj1.value().setName("changed 1");
  testObj2.value().setName("changed 42");
  print();
}

int main()
{
  testObj1.value().setName("main one");
  print();
  muduo::Thread t1(threadFunc);
  t1.start();
  t1.join();
  testObj2.value().setName("main two");
  print();

  pthread_exit(0);
}

/*
tid=4081, constructing 0xda9040
tid=4081, obj1 0xda9040 name=main one
tid=4081, constructing 0xda9060
tid=4081, obj2 0xda9060 name=
tid=4082, constructing 0x7fe8880008c0
tid=4082, obj1 0x7fe8880008c0 name=
tid=4082, constructing 0x7fe8880008e0
tid=4082, obj2 0x7fe8880008e0 name=
tid=4082, obj1 0x7fe8880008c0 name=changed 1
tid=4082, obj2 0x7fe8880008e0 name=changed 42
tid=4082, destructing 0x7fe8880008c0 changed 1
tid=4082, destructing 0x7fe8880008e0 changed 42
tid=4081, obj1 0xda9040 name=main one
tid=4081, obj2 0xda9060 name=main two
tid=4081, destructing 0xda9040 main one
tid=4081, destructing 0xda9060 main two
*/