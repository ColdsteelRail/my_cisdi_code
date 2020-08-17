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
  // 打印constructing
  testObj1.value().setName("main one"); // line 76
  // 打印obj1 obj2，此时obj已经初始化name=main one
  // obj2未初始化，name为空
  print(); // line 77~79
  
  muduo::Thread t1(threadFunc); // line 80~86
  // t1调用print，只是tid为主线程id+1=t1, 打印constructing
  // t1的local未初始化，name都为空
  // 设置changed1 changed42
  // t1调用print 名字已经改变
  t1.start();
  // t1结束，析构obj1 obj2，打印destructing
  t1.join(); // line 87 88

  testObj2.value().setName("main two"); 
  // 打印，tid为主线程id
  print(); // line 89 90

  // 主线程终止，释放主线程的obj1 obj2，打印destructing
  pthread_exit(0); // line 91 92

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