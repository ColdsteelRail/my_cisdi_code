// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCALSINGLETON_H
#define MUDUO_BASE_THREADLOCALSINGLETON_H

#include "muduo/base/noncopyable.h"

#include <assert.h>
#include <pthread.h>

namespace muduo
{

template<typename T>
class ThreadLocalSingleton : noncopyable
{
    public:
        ThreadLocalSingleton() = delete;
        ~ThreadLocalSingleton() = delete;
    
        // 不需要加锁，因为每个线程都有__thread数据，不会造成竞争
        static T& instance()
        {
            if (!t_value_)
            {
                t_value_ = new T();
                // 不需要显示释放，利用TSD pkey自动释放
                deleter_.set(t_value_);
            }
            return *t_value_;
        }

        static T* pointer()
        {
            return t_value_;
        }    
    private:
        static void destructor(void* obj)
        {
            assert(obj == t_value_);
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy; (void) dummy;
            delete t_value_;
            t_value_ = 0;
        }

        // 嵌套类，借助TSD线程特定数据pthread_key_t实现
        // 将ThreadLocal的obj类型设置为__Thread POD类型
        class Deleter
        {
        public:
            Deleter()
            {
                // 线程退出，回调destructor，delete t_value_
                pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
            }

            ~Deleter()
            {
                pthread_key_delete(pkey_);
            }

            void set(T* newObj)
            {
                assert(pthread_getspecific(pkey_) == NULL);
                pthread_setspecific(pkey_, newObj);
            }

            pthread_key_t pkey_;
        };

        // 静态数据成员，符合单例模式

        static __thread T* t_value_; // POD类型数据，线程独有
        static Deleter deleter_;        // delete对象销毁时，通过pthread_key_create()调用destructor回调
                                        // 释放t_value_
};

template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;



} // namespace muduo

#endif  // MUDUO_BASE_THREADLOCALSINGLETON_H
