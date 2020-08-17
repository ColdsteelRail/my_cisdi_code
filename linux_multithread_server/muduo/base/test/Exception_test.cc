#include "muduo/base/CurrentThread.h"
#include "muduo/base/Exception.h"
#include <functional>
#include <vector>
#include <stdio.h>

class Bar
{
 public:
  void test(std::vector<std::string> names = {})
  {
    printf("Stack:\n%s\n", muduo::CurrentThread::stackTrace(true).c_str());
    [] {
      printf("Stack inside lambda:\n%s\n", muduo::CurrentThread::stackTrace(true).c_str());
    }();
    std::function<void()> func([] {
      printf("Stack inside std::function:\n%s\n", muduo::CurrentThread::stackTrace(true).c_str());
    });
    func();

    func = std::bind(&Bar::callback, this);
    func();

    throw muduo::Exception("oops");
  }

 private:
   void callback()
   {
     printf("Stack inside std::bind:\n%s\n", muduo::CurrentThread::stackTrace(true).c_str());
   }
};

void foo()
{
  Bar b;
  b.test();
}

int main()
{
  try
  {
    foo();
  }
  catch (const muduo::Exception& ex)
  {
    printf("reason: %s\n", ex.what());
    printf("stack trace:\n%s\n", ex.stackTrace());
  }
}

/*
Stack:
./exception_test(Bar::test(std::vector<std::string, std::allocator<std::string> >)+0x28) [0x4022b8]
./exception_test(foo()+0x32) [0x401cf2]
./exception_test(main+0x6) [0x401b66]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf5) [0x7f5d4ea86f45]
./exception_test() [0x401bf9]

Stack inside lambda:
./exception_test(Bar::test(std::vector<std::string, std::allocator<std::string> >)::{lambda()#1}::operator()() const+0x14) [0x402144]
./exception_test(Bar::test(std::vector<std::string, std::allocator<std::string> >)+0x5b) [0x4022eb]
./exception_test(foo()+0x32) [0x401cf2]
./exception_test(main+0x6) [0x401b66]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf5) [0x7f5d4ea86f45]
./exception_test() [0x401bf9]

Stack inside std::function:
./exception_test(std::_Function_handler<void (), Bar::test(std::vector<std::string, std::allocator<std::string> >)::{lambda()#2}>::_M_invoke(std::_Any_data const&)+0x14) [0x401ff4]
./exception_test(Bar::test(std::vector<std::string, std::allocator<std::string> >)+0x8f) [0x40231f]
./exception_test(foo()+0x32) [0x401cf2]
./exception_test(main+0x6) [0x401b66]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf5) [0x7f5d4ea86f45]
./exception_test() [0x401bf9]

Stack inside std::bind:
./exception_test(Bar::callback()+0x14) [0x401f54]
./exception_test(Bar::test(std::vector<std::string, std::allocator<std::string> >)+0x126) [0x4023b6]
./exception_test(foo()+0x32) [0x401cf2]
./exception_test(main+0x6) [0x401b66]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf5) [0x7f5d4ea86f45]
./exception_test() [0x401bf9]

reason: oops
stack trace:
./exception_test(_ZN5muduo9ExceptionC1ESs+0x2a) [0x4027ba]
./exception_test(_ZN3Bar4testESt6vectorISsSaISsEE+0x154) [0x4023e4]
./exception_test(_Z3foov+0x32) [0x401cf2]
./exception_test(main+0x6) [0x401b66]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf5) [0x7f5d4ea86f45]
./exception_test() [0x401bf9]

*/