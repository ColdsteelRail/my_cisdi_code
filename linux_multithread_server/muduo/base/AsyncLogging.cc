// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "muduo/base/AsyncLogging.h"
#include "muduo/base/LogFile.h"
#include "muduo/base/Timestamp.h"

#include <stdio.h>

using namespace muduo;

AsyncLogging::AsyncLogging(const string& basename,
                           off_t rollSize,
                           int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_()
{
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len)
{
  muduo::MutexLockGuard lock(mutex_);
  // 如果当前缓冲(currentBuffer_)剩余空间足够大
  // 直接把日志消息拷贝到缓冲
  if (currentBuffer_->avail() > len)
  {
    currentBuffer_->append(logline, len);
  }
  else
  {
    // 当前缓冲满，写入buffers_
    buffers_.push_back(std::move(currentBuffer_));

    // 试图将另一部分buffer移入当前缓冲
    if (nextBuffer_)
    {
      currentBuffer_ = std::move(nextBuffer_);
    }
    else
    {
      // nextbuffer为空
      currentBuffer_.reset(new Buffer); // Rarely happens
    }
    currentBuffer_->append(logline, len);
    // currentbuffer空间不足，唤醒后端线程
    cond_.notify();
  }
}

// 特殊情况：
// 1 后端线程停滞：
//   前端current next被写满，notify唤醒后端，后端得不到cpu使用权而停滞了一段时间
//   这段时间内，为current新分配了缓存
// 2 前端丢失notify
//   前端notify时后端正在执行其他代码块
//   但下次进入while（true），不等待（buffer_不为空）

void AsyncLogging::threadFunc()
{
  assert(running_ == true);
  latch_.countDown();
  LogFile output(basename_, rollSize_, false);
  // 准备两块空闲buffer，在临界区内交换
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);
  while (running_)
  {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());

    {
      muduo::MutexLockGuard lock(mutex_);

      if (buffers_.empty())  // unusual usage!
      {
        cond_.waitForSeconds(flushInterval_);
      }
      
      // currentbuffer -> buffer_
      buffers_.push_back(std::move(currentBuffer_));
      currentBuffer_ = std::move(newBuffer1);
      // { buffer = currentbuffer + nextbuffer } -> buffersToWrite
      buffersToWrite.swap(buffers_);
      if (!nextBuffer_)
      {
        nextBuffer_ = std::move(newBuffer2);
      }
    }

    assert(!buffersToWrite.empty());

    if (buffersToWrite.size() > 25)
    {
      char buf[256];
      snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
               Timestamp::now().toFormattedString().c_str(),
               buffersToWrite.size()-2);
      fputs(buf, stderr);
      output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
    }

    for (const auto& buffer : buffersToWrite)
    {
      // FIXME: use unbuffered stdio FILE ? or use ::writev ?
      output.append(buffer->data(), buffer->length());
    }

    if (buffersToWrite.size() > 2)
    {
      // drop non-bzero-ed buffers, avoid trashing
      buffersToWrite.resize(2);
    }

    if (!newBuffer1)
    {
      assert(!buffersToWrite.empty());
      newBuffer1 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer1->reset();
    }

    if (!newBuffer2)
    {
      assert(!buffersToWrite.empty());
      newBuffer2 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer2->reset();
    }

    buffersToWrite.clear();
    output.flush();
  }
  output.flush();
}

