#ifndef MUDUO_BASE_BLOKINGQUEUE_H
#define MUDUO_BASE_BLOKINGQUEUE_H

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"

#include <deque>
#include <assert.h>

namespace muduo
{

template<typename T>
class BlockingQueue : noncopyable
{
 public:
  BlockingQueue()
    :mutex_(),
    notEmpty_(mutex_),
    queue_()
    {
    }

  void put(const T& x)
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify();
  }

  void put(T&& x) // make std::move valid (then unique_ptr is safe)
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(std::move(x)); // wait morphing saves us
    notEmpty_.notify();
  }

  T take()
  {
    MutexLockGuard lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty())
    {
        notEmpty_.wait();
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return front;
  }

  size_t size() const // const下mutex必须可以改变因此mutable -> mutex_
  {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

 private:
  mutable MutexLock mutex_;
  Condition         notEmpty_ GUARDED_BY(mutex_);
  std::deque<T>     queue_ GUARDED_BY(mutex_);
};

} // namspace muduo

#endif
