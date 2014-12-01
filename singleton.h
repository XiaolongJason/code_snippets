#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <boost/thread/once.hpp>

#include "marco.h"

template<typename T>
class Singleton
{
public:
  static T* Instance()
  {
    boost::call_once(Init, once_);
    return instance_;
  }

private:
  static void Init()
  {
    instance_ = new T;
    atexit(Destory);
  }

  static void Destory()
  {
    if (instance_)
    {
      delete instance_;
      instance_ = NULL;
    }
  }

private:
  static T* instance_;
  static boost::once_flag once_;

private:
  DISALLOW_COPY_AND_ASSIGN(Singleton);
};

template <typename T>
T* Singleton<T>::instance_ = NULL;

template <typename T>
boost::once_flag Singleton<T>::once_ = BOOST_ONCE_INIT;

#endif //_SINGLETON_H_
