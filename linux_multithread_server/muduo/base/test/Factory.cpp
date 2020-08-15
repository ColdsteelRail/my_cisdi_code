#include <map>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <../Mutex.h>

#include <assert.h>
#include <stdio.h>

using std::string;

namespace version1
{

// questionable code
class StockFactory : boost::noncopyable
{
public:

    boost::shared_ptr<Stock> get(const string& key) 
    {
        muduo::MutexLockGaurd lock(mutex_);
        boost::shared_ptr<Stock>& pStock = stocks_[key];
        if(!pStock) 
        {
            pStock.reset(new Stock(key));
        }
        return pStock;
    }

private:
    mutable muduo::MutexLockGuard mutex_;
    std::map<string, boost::shared_ptr<Stock> > stocks_;
};

}

namespace version2
{

// questionable code
class StockFactory : boost::noncopyable
{
public:
    boost::shared_ptr<Stock> get(const string& key) 
    {
        boost::shared_ptr<Stock> pStock;
        muduo::MutexLockGaurd lock(mutex_);
        boost::weak_ptr<Stock>& wkStock = stocks_[key];

        pStock = wkStock.lock();
        if(!pStock) 
        {
            pStock.reset(new Stock(key));
            wkStock = pStock;
        }
        return pStock;
    }

private:
    mutable muduo::MutexLockGuard mutex_;
    std::map<string, boost::weak_ptr<Stock> > stocks_;

};

}

namespace version3
{

// questionable code
class StockFactory : boost::noncopyable
{
public:
    boost::shared_ptr<Stock> get(const string& key) 
    {
        boost::shared_ptr<Stock> pStock;
        muduo::MutexLockGaurd lock(mutex_);
        boost::weak_ptr<Stock>& wkStock = stocks_[key];

        pStock = wkStock.lock();
        if(!pStock) 
        {
            pStock.reset(new Stock(key), 
                        boost::bind(&stockFactory::deleteStock, this, _1));
            wkStock = pStock;
        }
        return pStock;
    }

private:

    void deleteStock(Stock* stock)
    {
        printf("deleteStock[%p]\n",stock);
        if(stock)
        {
            muduo::MutexLockGuard lock(mutex_);
            stocks_.erase(stock->key());
        }
        delete stock;
    }

    mutable muduo::MutexLockGuard mutex_;
    std::map<string, boost::weak_ptr<Stock> > stocks_;

};

}

namespace version4
{
// Factory对象之恩那个是heap对象，并且只能由shared_ptr管理
class StockFactory : public boost::enable_shared_from_this<StockFactory>,
                            boost::noncopyable
{
public:
    boost::shared_ptr<Stock> get(const string& key) 
    {
        boost::shared_ptr<Stock> pStock;
        muduo::MutexLockGaurd lock(mutex_);
        boost::weak_ptr<Stock>& wkStock = stocks_[key];

        pStock = wkStock.lock();
        if(!pStock) 
        {
            pStock.reset(new Stock(key), 
                        boost::bind(&stockFactory::deleteStock, 
                        shared_from_this(),
                         _1));
            wkStock = pStock;
        }
        return pStock;
    }

private:

    void deleteStock(Stock* stock)
    {
        printf("deleteStock[%p]\n",stock);
        if(stock)
        {
            muduo::MutexLockGuard lock(mutex_);
            stocks_.erase(stock->key());
        }
        delete stock;
    }

    mutable muduo::MutexLockGuard mutex_;
    std::map<string, boost::weak_ptr<Stock> > stocks_;

};

}


class StockFactory : public boost::enable_shared_from_this<StockFactory>,
                            boost::noncopyable
{
public:
    boost::shared_ptr<Stock> get(const string& key) 
    {
        boost::shared_ptr<Stock> pStock;
        muduo::MutexLockGaurd lock(mutex_);
        boost::weak_ptr<Stock>& wkStock = stocks_[key];

        pStock = wkStock.lock();
        if(!pStock) 
        {
            pStock.reset(new Stock(key), 
                        boost::bind(&stockFactory::weakDeleteCallback, 
                        boost::weak_ptr<StockFactory>(shared_from_this()),
                         _1));
            wkStock = pStock;
        }
        return pStock;
    }

private:
    static void weakDeleteCallback(const boost::weak_ptr<StockFactory>& wkFactory,
                                    Stock* stock)
    {
      printf("weakDeleteCallback[%p]\n", stock);
      boost::shared_ptr<StockFacktory> factory(wkFactory.lock());
      if(factory)
      {
        factory->removeStock(stock);
      }
      else
      {
        printf("factory died.\n");
      }
      delete stock;
    }

    void deleteStock(Stock* stock)
    {
        printf("deleteStock[%p]\n",stock);
        if(stock)
        {
            muduo::MutexLockGuard lock(mutex_);
            stocks_.erase(stock->key());
        }
        delete stock;
    }

    mutable muduo::MutexLockGuard mutex_;
    std::map<string, boost::weak_ptr<Stock> > stocks_;

};


namespace boost {

  class bad_weak_ptr: public std::exception;

  template<class T> class weak_ptr;

  template<class T> class shared_ptr {
  public:

    typedef /*see below*/ element_type;

    constexpr shared_ptr() noexcept;
    constexpr shared_ptr(std::nullptr_t) noexcept;

    template<class Y> explicit shared_ptr(Y * p);
    template<class Y, class D> shared_ptr(Y * p, D d);
    template<class Y, class D, class A> shared_ptr(Y * p, D d, A a);
    template<class D> shared_ptr(std::nullptr_t p, D d);
    template<class D, class A> shared_ptr(std::nullptr_t p, D d, A a);

    ~shared_ptr() noexcept;

    shared_ptr(shared_ptr const & r) noexcept;
    template<class Y> shared_ptr(shared_ptr<Y> const & r) noexcept;

    shared_ptr(shared_ptr && r) noexcept;
    template<class Y> shared_ptr(shared_ptr<Y> && r) noexcept;

    template<class Y> shared_ptr(shared_ptr<Y> const & r, element_type * p) noexcept;
    template<class Y> shared_ptr(shared_ptr<Y> && r, element_type * p) noexcept;

    template<class Y> explicit shared_ptr(weak_ptr<Y> const & r);

    template<class Y> explicit shared_ptr(std::auto_ptr<Y> & r);
    template<class Y> shared_ptr(std::auto_ptr<Y> && r);

    template<class Y, class D> shared_ptr(std::unique_ptr<Y, D> && r);

    shared_ptr & operator=(shared_ptr const & r) noexcept;
    template<class Y> shared_ptr & operator=(shared_ptr<Y> const & r) noexcept;

    shared_ptr & operator=(shared_ptr const && r) noexcept;
    template<class Y> shared_ptr & operator=(shared_ptr<Y> const && r) noexcept;

    template<class Y> shared_ptr & operator=(std::auto_ptr<Y> & r);
    template<class Y> shared_ptr & operator=(std::auto_ptr<Y> && r);

    template<class Y, class D> shared_ptr & operator=(std::unique_ptr<Y, D> && r);

    shared_ptr & operator=(std::nullptr_t) noexcept;

    void reset() noexcept;

    template<class Y> void reset(Y * p);
    template<class Y, class D> void reset(Y * p, D d);
    template<class Y, class D, class A> void reset(Y * p, D d, A a);

    template<class Y> void reset(shared_ptr<Y> const & r, element_type * p) noexcept;
    template<class Y> void reset(shared_ptr<Y> && r, element_type * p) noexcept;

    T & operator*() const noexcept; // only valid when T is not an array type
    T * operator->() const noexcept; // only valid when T is not an array type

    // only valid when T is an array type
    element_type & operator[](std::ptrdiff_t i) const noexcept;

    element_type * get() const noexcept;

    bool unique() const noexcept;
    long use_count() const noexcept;

    explicit operator bool() const noexcept;

    void swap(shared_ptr & b) noexcept;

    template<class Y> bool owner_before(shared_ptr<Y> const & rhs) const noexcept;
    template<class Y> bool owner_before(weak_ptr<Y> const & rhs) const noexcept;
  };

  template<class T, class U>
    bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b) noexcept;

  template<class T, class U>
    bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b) noexcept;

  template<class T, class U>
    bool operator<(shared_ptr<T> const & a, shared_ptr<U> const & b) noexcept;

  template<class T> bool operator==(shared_ptr<T> const & p, std::nullptr_t) noexcept;
  template<class T> bool operator==(std::nullptr_t, shared_ptr<T> const & p) noexcept;

  template<class T> bool operator!=(shared_ptr<T> const & p, std::nullptr_t) noexcept;
  template<class T> bool operator!=(std::nullptr_t, shared_ptr<T> const & p) noexcept;

  template<class T> void swap(shared_ptr<T> & a, shared_ptr<T> & b) noexcept;

  template<class T>
    typename shared_ptr<T>::element_type *
      get_pointer(shared_ptr<T> const & p) noexcept;

  template<class T, class U>
    shared_ptr<T> static_pointer_cast(shared_ptr<U> const & r) noexcept;

  template<class T, class U>
    shared_ptr<T> const_pointer_cast(shared_ptr<U> const & r) noexcept;

  template<class T, class U>
    shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const & r) noexcept;

  template<class T, class U>
    shared_ptr<T> reinterpret_pointer_cast(shared_ptr<U> const & r) noexcept;

  template<class E, class T, class Y>
    std::basic_ostream<E, T> &
      operator<< (std::basic_ostream<E, T> & os, shared_ptr<Y> const & p);

  template<class D, class T> D * get_deleter(shared_ptr<T> const & p) noexcept;

  template<class T> bool atomic_is_lock_free( shared_ptr<T> const * p ) noexcept;

  template<class T> shared_ptr<T> atomic_load( shared_ptr<T> const * p ) noexcept;
  template<class T>
    shared_ptr<T> atomic_load_explicit( shared_ptr<T> const * p, int ) noexcept;

  template<class T>
    void atomic_store( shared_ptr<T> * p, shared_ptr<T> r ) noexcept;
  template<class T>
    void atomic_store_explicit( shared_ptr<T> * p, shared_ptr<T> r, int ) noexcept;

  template<class T>
    shared_ptr<T> atomic_exchange( shared_ptr<T> * p, shared_ptr<T> r ) noexcept;
  template<class T>
    shared_ptr<T> atomic_exchange_explicit(
      shared_ptr<T> * p, shared_ptr<T> r, int ) noexcept;

  template<class T>
    bool atomic_compare_exchange(
      shared_ptr<T> * p, shared_ptr<T> * v, shared_ptr<T> w ) noexcept;
  template<class T>
    bool atomic_compare_exchange_explicit(
      shared_ptr<T> * p, shared_ptr<T> * v, shared_ptr<T> w, int, int ) noexcept;
}