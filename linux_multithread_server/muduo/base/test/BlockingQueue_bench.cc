#include "muduo/base/BlockingQueue.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Thread.h"
#include "muduo/base/Timestamp.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>

class Bench
{
 public:
  Bench(int numThreads)
    : latch_(numThreads)
  {
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.emplace_back(new muduo::Thread(
            std::bind(&Bench::threadFunc, this), muduo::string(name)));
    }
    for (auto& thr : threads_)
    {
      thr->start();
    }
  }

  void run(int times)
  {
    printf("waiting for count down latch\n");
    latch_.wait();
    printf("all threads started\n");
    for (int i = 0; i < times; ++i)
    {
      muduo::Timestamp now(muduo::Timestamp::now());
      queue_.put(now);
      usleep(1000);
    }
  }

  void joinAll()
  {
    for (size_t i = 0; i < threads_.size(); ++i)
    {
      queue_.put(muduo::Timestamp::invalid());
    }

    for (auto& thr : threads_)
    {
      thr->join();
    }
  }

 private:

  void threadFunc()
  {
    printf("tid=%d, %s started\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());

    std::unordered_map<int, int> delays;
    latch_.countDown();
    bool running = true;
    while (running)
    {
      muduo::Timestamp t(queue_.take());
      muduo::Timestamp now(muduo::Timestamp::now());
      if (t.valid())
      {
        int delay = static_cast<int>(timeDifference(now, t) * 1000000);
        // printf("tid=%d, latency = %d us\n",
        //        muduo::CurrentThread::tid(), delay);
        ++delays[delay];
      }
      running = t.valid();
    }

    printf("tid=%d, %s stopped\n",
           muduo::CurrentThread::tid(),
           muduo::CurrentThread::name());
    for (const auto& delay : delays)
    {
      printf("tid = %d, delay = %d, count = %d\n",
             muduo::CurrentThread::tid(),
             delay.first, delay.second);
    }
  }

  muduo::BlockingQueue<muduo::Timestamp> queue_;
  muduo::CountDownLatch latch_;
  std::vector<std::unique_ptr<muduo::Thread>> threads_;
};

int main(int argc, char* argv[])
{
  int threads = 2;//argc > 1 ? atoi(argv[1]) : 1;

  Bench t(threads);
  t.run(10000);
  t.joinAll();
}
/*
tid=4095, work thread 0 started
waiting for count down latch
tid=4096, work thread 1 started
all threads started
tid=4096, work thread 1 stopped
tid = 4096, delay = 4, count = 1
tid = 4096, delay = 5, count = 60
tid = 4096, delay = 6, count = 266
tid = 4096, delay = 7, count = 598
tid = 4096, delay = 8, count = 490
tid = 4096, delay = 9, count = 294
tid = 4096, delay = 10, count = 236
tid = 4096, delay = 11, count = 467
tid = 4096, delay = 12, count = 891
tid = 4096, delay = 13, count = 399
tid = 4096, delay = 14, count = 209
tid = 4096, delay = 15, count = 100
tid = 4096, delay = 16, count = 86
tid = 4096, delay = 17, count = 35
tid = 4096, delay = 18, count = 32
tid = 4096, delay = 19, count = 50
tid = 4096, delay = 20, count = 87
tid = 4096, delay = 21, count = 62
tid = 4096, delay = 22, count = 55
tid = 4096, delay = 23, count = 37
tid = 4096, delay = 24, count = 37
tid = 4096, delay = 25, count = 57
tid = 4096, delay = 26, count = 66
tid = 4096, delay = 27, count = 52
tid = 4096, delay = 28, count = 33
tid = 4096, delay = 29, count = 24
tid = 4096, delay = 30, count = 20
tid = 4096, delay = 31, count = 32
tid = 4096, delay = 32, count = 23
tid = 4096, delay = 33, count = 21
tid = 4096, delay = 34, count = 16
tid = 4096, delay = 35, count = 12
tid = 4096, delay = 36, count = 5
tid = 4096, delay = 37, count = 11
tid = 4096, delay = 38, count = 8
tid = 4096, delay = 39, count = 10
tid = 4096, delay = 40, count = 9
tid = 4096, delay = 41, count = 6
tid = 4096, delay = 42, count = 7
tid = 4096, delay = 43, count = 9
tid = 4096, delay = 44, count = 9
tid = 4096, delay = 45, count = 5
tid = 4096, delay = 46, count = 4
tid = 4096, delay = 47, count = 7
tid = 4096, delay = 48, count = 3
tid = 4096, delay = 49, count = 3
tid = 4096, delay = 50, count = 3
tid = 4096, delay = 51, count = 5
tid = 4096, delay = 52, count = 3
tid = 4096, delay = 53, count = 4
tid = 4096, delay = 54, count = 1
tid = 4096, delay = 55, count = 4
tid = 4096, delay = 56, count = 3
tid = 4096, delay = 57, count = 2
tid = 4096, delay = 58, count = 2
tid = 4096, delay = 59, count = 2
tid = 4096, delay = 60, count = 1
tid = 4096, delay = 61, count = 1
tid = 4096, delay = 62, count = 2
tid = 4096, delay = 63, count = 1
tid = 4096, delay = 64, count = 3
tid = 4096, delay = 65, count = 2
tid = 4096, delay = 66, count = 1
tid = 4096, delay = 67, count = 1
tid = 4096, delay = 69, count = 1
tid = 4096, delay = 70, count = 1
tid = 4096, delay = 71, count = 1
tid = 4096, delay = 75, count = 1
tid = 4096, delay = 78, count = 1
tid = 4096, delay = 84, count = 2
tid = 4096, delay = 86, count = 2
tid = 4096, delay = 91, count = 1
tid = 4096, delay = 100, count = 1
tid = 4096, delay = 111, count = 1
tid = 4096, delay = 132, count = 1
tid = 4096, delay = 156, count = 1
tid=4095, work thread 0 stopped
tid = 4095, delay = 4, count = 2
tid = 4095, delay = 5, count = 80
tid = 4095, delay = 6, count = 218
tid = 4095, delay = 7, count = 576
tid = 4095, delay = 8, count = 449
tid = 4095, delay = 9, count = 292
tid = 4095, delay = 10, count = 243
tid = 4095, delay = 11, count = 588
tid = 4095, delay = 12, count = 774
tid = 4095, delay = 13, count = 374
tid = 4095, delay = 14, count = 213
tid = 4095, delay = 15, count = 129
tid = 4095, delay = 16, count = 89
tid = 4095, delay = 17, count = 72
tid = 4095, delay = 18, count = 46
tid = 4095, delay = 19, count = 54
tid = 4095, delay = 20, count = 88
tid = 4095, delay = 21, count = 72
tid = 4095, delay = 22, count = 56
tid = 4095, delay = 23, count = 49
tid = 4095, delay = 24, count = 42
tid = 4095, delay = 25, count = 59
tid = 4095, delay = 26, count = 61
tid = 4095, delay = 27, count = 47
tid = 4095, delay = 28, count = 40
tid = 4095, delay = 29, count = 21
tid = 4095, delay = 30, count = 35
tid = 4095, delay = 31, count = 27
tid = 4095, delay = 32, count = 27
tid = 4095, delay = 33, count = 20
tid = 4095, delay = 34, count = 12
tid = 4095, delay = 35, count = 10
tid = 4095, delay = 36, count = 11
tid = 4095, delay = 37, count = 12
tid = 4095, delay = 38, count = 7
tid = 4095, delay = 39, count = 5
tid = 4095, delay = 40, count = 8
tid = 4095, delay = 41, count = 4
tid = 4095, delay = 42, count = 2
tid = 4095, delay = 43, count = 6
tid = 4095, delay = 44, count = 4
tid = 4095, delay = 45, count = 6
tid = 4095, delay = 47, count = 4
tid = 4095, delay = 48, count = 5
tid = 4095, delay = 49, count = 12
tid = 4095, delay = 50, count = 5
tid = 4095, delay = 51, count = 2
tid = 4095, delay = 52, count = 2
tid = 4095, delay = 53, count = 2
tid = 4095, delay = 54, count = 3
tid = 4095, delay = 55, count = 2
tid = 4095, delay = 56, count = 1
tid = 4095, delay = 57, count = 3
tid = 4095, delay = 59, count = 2
tid = 4095, delay = 60, count = 2
tid = 4095, delay = 61, count = 1
tid = 4095, delay = 62, count = 3
tid = 4095, delay = 63, count = 3
tid = 4095, delay = 64, count = 1
tid = 4095, delay = 66, count = 2
tid = 4095, delay = 67, count = 2
tid = 4095, delay = 69, count = 1
tid = 4095, delay = 70, count = 1
tid = 4095, delay = 71, count = 2
tid = 4095, delay = 72, count = 1
tid = 4095, delay = 73, count = 1
tid = 4095, delay = 80, count = 2
tid = 4095, delay = 84, count = 2
tid = 4095, delay = 94, count = 1
tid = 4095, delay = 103, count = 1
tid = 4095, delay = 132, count = 1
tid = 4095, delay = 149, count = 1
*/
