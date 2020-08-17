#include "muduo/base/ThreadPool.h"

#include "muduo/base/Exception.h"

#include <assert.h>
#include <stdio.h>

using namespace muduo;

ThreadPool::ThreadPool(const string& nameArg)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(nameArg),
      maxQueueSize_(0),
      running_(false)
{
}

ThreadPool::~ThreadPool()
{
    // 线程池中的线程生命周期必须短于线程池对象生命周期
    // 因此必须销毁线程
    if(running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty()); // 初始为空
    running_ = true;
    threads_.reserve(numThreads);

    // 创建线程
    for (int i = 0; i < numThreads; ++i)
    {
        char id[32];
        snprintf(id, sizeof id, "%d", i+1);
        threads_.emplace_back(new muduo::Thread(
            // runInThread循环take()取task
            std::bind(&ThreadPool::runInThread, this), name_+id));
        threads_[i]->start();
    }

    // 主线程执行threadInitCallback_,通过setThreadInitCallback()设置
    if(numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    // notEmpy_被唤醒，任务队列全部出队
    {
        // 局部锁
        MutexLockGuard lock(mutex_);
        notEmpty_.notifyAll();
        // take() 中用到
        running_ = false;
    }
    for(auto& thr : threads_)
    {
        thr->join();
    }

}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

void ThreadPool::run(Task task)
{
    // task压入任务队列
    // 若线程池没有线程，主线程执行
    // 如果任务队列满，等待出队
    // notEmpty_被唤醒，保证消费端一定能执行
    if(threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        while(isFull())
        {
            notFull_.wait();
        }
        assert(!isFull());
        queue_.push_back(std::move(task));
        notEmpty_.notify();
    }
    
}

ThreadPool::Task ThreadPool::take()
{
    // 让一个任务出队，并返回此任务
    MutexLockGuard lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while(queue_.empty() && running_)
    {
        notEmpty_.wait();
    }
    Task task;
    // 这里判断一下，start()可能在空的情况唤醒notEmpty_
    if(!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        if(maxQueueSize_ > 0)
        {
            notFull_.notify();
        }
    }
    return task;
}

bool ThreadPool::isFull() const
{
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try
    {
        if(threadInitCallback_)
        {
            threadInitCallback_();
        }
        while(running_)
        {
            // 任务队列取task
            Task task(take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const Exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}

