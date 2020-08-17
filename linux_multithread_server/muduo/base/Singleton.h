#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include "muduo/base/noncopyable.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h> // atexit

namespace muduo
{

namespace detail
{

template<typename T>
struct has_no_destroy
{
    template <typename C> static char test(decltype(&C::no_destroy));
    template <typename C> static int32_t test(...);
    const static bool value = sizeof(test<T>(0))== 1;
};

} // namespace detail

template<typename T>
class Singleton : noncopyable
{
    public:
        Singleton() = delete;
        ~Singleton() = delete;

        static T& instance()
        {
            // pthread_once 保证只执行一次，线程安全，高效
            pthread_once(&ponce_, &Singleton::init);
            assert(value_ != NULL);
            return *value_;
        }

    private:
        static void init()
        {
            value_ = new T();
            
            if (!detail::has_no_destroy<T>::value)
            {
                // 在整个程序结束时调用destroy
                ::atexit(destroy);
            }
        }
    
        static void destroy()
        {
            // 销毁对象，一定是完全类型
            // 非完全类型：例如class A{}; A* p; delete p; A则不是一个完全类型
            // 如果是不完全类型，那么sizeof(T) = 0, 那么会报错
            typedef char T_must_be_conmpete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_conmpete_type dummy; (void) dummy; // dummy要使用一下，否则报错
            
            delete value_;
            value_ = NULL;
        }

    private:
        static pthread_once_t ponce_;
        static T*             value_; 
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

} // namespace muduo

#endif
