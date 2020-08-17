// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCAL_H
#define MUDUO_BASE_THREADLOCAL_H

#include "muduo/base/Mutex.h"
#include "muduo/base/noncopyable.h"

#include <pthread.h>

namespace muduo
{

template<typename T>
class ThreadLocal : noncopyable
{

    public:
        ThreadLocal()
        {
            // 回调函数destrucot来删除实际数据T
            MCHECK(pthread_key_create(&pkey_, &ThreadLocal::destructor));
        }

        ~ThreadLocal()
        {
            // 仅仅销毁kye,并没有销毁实际数据
            MCHECK(pthread_key_delete(pkey_));
        }

        T& value()
        {
            // get获取 set设置
            T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
            if (!perThreadValue)
            {
                T* newObj = new T();
                MCHECK(pthread_setspecific(pkey_, newObj));
                perThreadValue = newObj;
            }
            return *perThreadValue;
        }

    private:
        static void destructor(void* x)
        {
            T* obj = static_cast<T*>(x);
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy; (void) dummy;
            delete obj;
        }

    private:
        // 线程特定数据
        // pthread_key_t 使用后，每个线程都将被动拥有，但存储数据是私有的
        // pthread_key_t 中的数据是堆栈式的
        pthread_key_t pkey_;
};

} // namespace muduo

#endif
