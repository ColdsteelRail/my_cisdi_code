#include "muduo/base/BoundedBlockingQueue.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Thread.h"

#include <string>
#include <vector>

#include <stdio.h>
#include <unistd.h>

class Test
{
    public:
        Test(int numThreads)
            // 必须按照声明的顺序初始化
            : queue_(20),
              latch_(numThreads)
        {
            threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.emplace_back(new muduo::Thread(
            std::bind(&Test::threadFunc, this), muduo::string(name)));
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
        char buf[32];
        snprintf(buf, sizeof buf, "hello %d", i);
        queue_.put(buf);
        printf("tid=%d, put data = %s, size = %zd\n", muduo::CurrentThread::tid(), buf, queue_.size());
        }
    }

    void joinAll()
    {
        for (size_t i = 0; i < threads_.size(); ++i)
        {
        queue_.put("stop");
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

            latch_.countDown();
            bool running = true;
            while (running)
            {
            std::string d(queue_.take());
            printf("tid=%d, get data = %s, size = %zd\n", muduo::CurrentThread::tid(), d.c_str(), queue_.size());
            running = (d != "stop");
            }

            printf("tid=%d, %s stopped\n",
            muduo::CurrentThread::tid(),
            muduo::CurrentThread::name());
        }

        std::vector<std::unique_ptr<muduo::Thread>> threads_;
        muduo::BoundedBlockingQueue<std::string> queue_;
        muduo::CountDownLatch latch_;
};
void testMove()
{
#if BOOST_VERSION >= 105500L
    muduo::BoundedBlockingQueue<std::unique_ptr<int>> queue(10);
    queue.put(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x = queue.take();
    printf("took %d\n", *x);
    *x = 123;
    queue.put(std::move(x));
    std::unique_ptr<int> y;
    y = queue.take();
    printf("took %d\n", *y);
#endif
}

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());
    testMove();
    Test t(5);
    t.run(100);
    t.joinAll();

    printf("number of created threads %d\n", muduo::Thread::numCreated());
}

/*
pid=4149, tid=4149
tid=4150, work thread 0 started
tid=4151, work thread 1 started
tid=4152, work thread 2 started
tid=4153, work thread 3 started
waiting for count down latch
tid=4154, work thread 4 started
all threads started
tid=4149, put data = hello 0, size = 1
tid=4149, put data = hello 1, size = 2
tid=4149, put data = hello 2, size = 3
tid=4149, put data = hello 3, size = 4
tid=4149, put data = hello 4, size = 5
tid=4149, put data = hello 5, size = 6
tid=4149, put data = hello 6, size = 7
tid=4149, put data = hello 7, size = 8
tid=4149, put data = hello 8, size = 9
tid=4149, put data = hello 9, size = 10
tid=4149, put data = hello 10, size = 11
tid=4149, put data = hello 11, size = 12
tid=4149, put data = hello 12, size = 13
tid=4149, put data = hello 13, size = 14
tid=4149, put data = hello 14, size = 15
tid=4149, put data = hello 15, size = 16
tid=4149, put data = hello 16, size = 17
tid=4149, put data = hello 17, size = 18
tid=4149, put data = hello 18, size = 19
tid=4149, put data = hello 19, size = 20
tid=4150, get data = hello 0, size = 19
tid=4150, get data = hello 1, size = 19
tid=4150, get data = hello 2, size = 18
tid=4150, get data = hello 3, size = 17
tid=4150, get data = hello 4, size = 16
tid=4150, get data = hello 5, size = 15
tid=4150, get data = hello 6, size = 14
tid=4150, get data = hello 7, size = 13
tid=4150, get data = hello 8, size = 12
tid=4150, get data = hello 9, size = 11
tid=4150, get data = hello 10, size = 10
tid=4150, get data = hello 11, size = 9
tid=4150, get data = hello 12, size = 8
tid=4150, get data = hello 13, size = 7
tid=4150, get data = hello 14, size = 6
tid=4150, get data = hello 15, size = 5
tid=4150, get data = hello 16, size = 4
tid=4150, get data = hello 17, size = 3
tid=4150, get data = hello 18, size = 2
tid=4150, get data = hello 19, size = 1
tid=4150, get data = hello 20, size = 0
tid=4149, put data = hello 20, size = 20
tid=4149, put data = hello 21, size = 1
tid=4149, put data = hello 22, size = 2
tid=4149, put data = hello 23, size = 3
tid=4149, put data = hello 24, size = 4
tid=4149, put data = hello 25, size = 5
tid=4149, put data = hello 26, size = 6
tid=4149, put data = hello 27, size = 7
tid=4149, put data = hello 28, size = 8
tid=4149, put data = hello 29, size = 9
tid=4149, put data = hello 30, size = 10
tid=4149, put data = hello 31, size = 11
tid=4149, put data = hello 32, size = 12
tid=4149, put data = hello 33, size = 13
tid=4149, put data = hello 34, size = 14
tid=4149, put data = hello 35, size = 15
tid=4149, put data = hello 36, size = 16
tid=4149, put data = hello 37, size = 17
tid=4149, put data = hello 38, size = 18
tid=4149, put data = hello 39, size = 19
tid=4149, put data = hello 40, size = 20
tid=4150, get data = hello 21, size = 19
tid=4150, get data = hello 22, size = 19
tid=4150, get data = hello 23, size = 18
tid=4150, get data = hello 24, size = 17
tid=4150, get data = hello 25, size = 16
tid=4150, get data = hello 26, size = 15
tid=4150, get data = hello 27, size = 14
tid=4150, get data = hello 28, size = 13
tid=4150, get data = hello 29, size = 12
tid=4150, get data = hello 30, size = 11
tid=4150, get data = hello 31, size = 10
tid=4150, get data = hello 32, size = 9
tid=4150, get data = hello 33, size = 8
tid=4150, get data = hello 34, size = 7
tid=4150, get data = hello 35, size = 6
tid=4150, get data = hello 36, size = 5
tid=4150, get data = hello 37, size = 4
tid=4150, get data = hello 38, size = 3
tid=4150, get data = hello 39, size = 2
tid=4150, get data = hello 40, size = 1
tid=4150, get data = hello 41, size = 0
tid=4149, put data = hello 41, size = 20
tid=4149, put data = hello 42, size = 1
tid=4149, put data = hello 43, size = 2
tid=4149, put data = hello 44, size = 3
tid=4149, put data = hello 45, size = 4
tid=4149, put data = hello 46, size = 5
tid=4149, put data = hello 47, size = 6
tid=4149, put data = hello 48, size = 7
tid=4149, put data = hello 49, size = 8
tid=4149, put data = hello 50, size = 9
tid=4149, put data = hello 51, size = 10
tid=4149, put data = hello 52, size = 11
tid=4149, put data = hello 53, size = 12
tid=4149, put data = hello 54, size = 13
tid=4149, put data = hello 55, size = 14
tid=4149, put data = hello 56, size = 15
tid=4149, put data = hello 57, size = 16
tid=4149, put data = hello 58, size = 17
tid=4149, put data = hello 59, size = 18
tid=4149, put data = hello 60, size = 19
tid=4149, put data = hello 61, size = 20
tid=4150, get data = hello 42, size = 19
tid=4150, get data = hello 43, size = 19
tid=4150, get data = hello 44, size = 18
tid=4150, get data = hello 45, size = 17
tid=4150, get data = hello 46, size = 16
tid=4150, get data = hello 47, size = 15
tid=4150, get data = hello 48, size = 14
tid=4150, get data = hello 49, size = 13
tid=4150, get data = hello 50, size = 12
tid=4150, get data = hello 51, size = 11
tid=4150, get data = hello 52, size = 10
tid=4150, get data = hello 53, size = 9
tid=4150, get data = hello 54, size = 8
tid=4150, get data = hello 55, size = 7
tid=4150, get data = hello 56, size = 6
tid=4150, get data = hello 57, size = 5
tid=4150, get data = hello 58, size = 4
tid=4150, get data = hello 59, size = 3
tid=4150, get data = hello 60, size = 2
tid=4150, get data = hello 61, size = 1
tid=4150, get data = hello 62, size = 0
tid=4149, put data = hello 62, size = 20
tid=4149, put data = hello 63, size = 1
tid=4149, put data = hello 64, size = 2
tid=4149, put data = hello 65, size = 3
tid=4149, put data = hello 66, size = 4
tid=4149, put data = hello 67, size = 5
tid=4149, put data = hello 68, size = 6
tid=4149, put data = hello 69, size = 7
tid=4149, put data = hello 70, size = 8
tid=4149, put data = hello 71, size = 9
tid=4149, put data = hello 72, size = 10
tid=4149, put data = hello 73, size = 11
tid=4149, put data = hello 74, size = 12
tid=4149, put data = hello 75, size = 13
tid=4149, put data = hello 76, size = 14
tid=4149, put data = hello 77, size = 15
tid=4149, put data = hello 78, size = 16
tid=4149, put data = hello 79, size = 17
tid=4149, put data = hello 80, size = 18
tid=4149, put data = hello 81, size = 19
tid=4149, put data = hello 82, size = 20
tid=4150, get data = hello 63, size = 19
tid=4150, get data = hello 64, size = 19
tid=4150, get data = hello 65, size = 18
tid=4150, get data = hello 66, size = 17
tid=4150, get data = hello 67, size = 16
tid=4150, get data = hello 68, size = 15
tid=4150, get data = hello 69, size = 14
tid=4150, get data = hello 70, size = 13
tid=4150, get data = hello 71, size = 12
tid=4150, get data = hello 72, size = 11
tid=4150, get data = hello 73, size = 10
tid=4150, get data = hello 74, size = 9
tid=4150, get data = hello 75, size = 8
tid=4150, get data = hello 76, size = 7
tid=4150, get data = hello 77, size = 6
tid=4150, get data = hello 78, size = 5
tid=4150, get data = hello 79, size = 4
tid=4150, get data = hello 80, size = 3
tid=4150, get data = hello 81, size = 2
tid=4150, get data = hello 82, size = 1
tid=4150, get data = hello 83, size = 0
tid=4149, put data = hello 83, size = 20
tid=4149, put data = hello 84, size = 1
tid=4149, put data = hello 85, size = 2
tid=4149, put data = hello 86, size = 3
tid=4149, put data = hello 87, size = 4
tid=4149, put data = hello 88, size = 5
tid=4149, put data = hello 89, size = 6
tid=4149, put data = hello 90, size = 7
tid=4149, put data = hello 91, size = 8
tid=4149, put data = hello 92, size = 9
tid=4149, put data = hello 93, size = 10
tid=4149, put data = hello 94, size = 11
tid=4149, put data = hello 95, size = 12
tid=4149, put data = hello 96, size = 13
tid=4149, put data = hello 97, size = 14
tid=4149, put data = hello 98, size = 15
tid=4149, put data = hello 99, size = 16
tid=4150, get data = hello 84, size = 19
tid=4150, get data = hello 85, size = 19
tid=4150, get data = hello 86, size = 18
tid=4150, get data = hello 87, size = 17
tid=4150, get data = hello 88, size = 16
tid=4150, get data = hello 89, size = 15
tid=4150, get data = hello 90, size = 14
tid=4150, get data = hello 91, size = 13
tid=4150, get data = hello 92, size = 12
tid=4150, get data = hello 93, size = 11
tid=4150, get data = hello 94, size = 10
tid=4150, get data = hello 95, size = 9
tid=4150, get data = hello 96, size = 8
tid=4150, get data = hello 97, size = 7
tid=4150, get data = hello 98, size = 6
tid=4150, get data = hello 99, size = 5
tid=4150, get data = stop, size = 4
tid=4150, work thread 0 stopped
tid=4151, get data = stop, size = 3
tid=4151, work thread 1 stopped
tid=4152, get data = stop, size = 2
tid=4152, work thread 2 stopped
tid=4153, get data = stop, size = 1
tid=4153, work thread 3 stopped
tid=4154, get data = stop, size = 0
tid=4154, work thread 4 stopped
number of created threads 5
*/