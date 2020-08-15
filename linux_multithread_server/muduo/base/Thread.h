#ifndef HEX_BASE_THREAD_H
#define HEX_BASE_THREAD_H

#include <pthread.h>
#include <functional>
#include <iostream>

namespace hex
{

class Thread
{
 public:
   typedef std::function<void ()> ThreadFunc;

   explicit Thread(const ThreadFunc&, const std::string& name = std::string());
   ~Thread();

   void start();
   void join();

   bool started() const { return started_; }

   pid_t tid() const { return *tid_; }
   const std::string& name() const { return name_; }

   static int numCreated() { return numCreated_.get(); }

private:
  bool          started_;
  bool          joined_;
  pthread_t     pthreadId_;
  std::shared_ptr<pid_t> tid_;
  ThreadFunc    func_;
  std::string   name_;

  static std::atomic<int> numCreated_;
};

namespace CurrentThread
{
  pid_t tid();
  const char* name();
  bool isMainThread();
}

}

#endif