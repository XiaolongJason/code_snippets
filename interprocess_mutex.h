#ifndef _INTERPROCESS_MUTEX_H_
#define _INTERPROCESS_MUTEX_H_

#include "windows.h"
#include <iostream>

#include "tstring.h"
#include "marco.h"

//  ScopedLock<InterprocessMutex>   lock;

template <class T>
class ScopedLock
{
public:
  explicit ScopedLock(T& mutex) : mutex_(mutex)
  {
    mutex_.Lock();
  }

  ~ScopedLock()
  {
    mutex_.Unlock();
  }

private:
  T & mutex_;

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedLock);
};


class InterprocessMutex
{
public:
  InterprocessMutex(const tstring& name)
    :name_(name)
  {
    mutex_ = CreateMutex(NULL, FALSE, name_.c_str());
    if (!mutex_)
    {
      std::cout << "CreateMutex Error :" << name_ << std::endl;
    }
  }

  ~InterprocessMutex()
  {
    if (mutex_)
    {
      CloseHandle(mutex_);
    }
  }

  void Lock()
  {
    if (mutex_)
    {
      WaitForSingleObject(mutex_, INFINITE);
    }
  }
  
  void Unlock()
  {
    if (mutex_)
    {
      ReleaseMutex(mutex_);
    }
  }

private:
  tstring name_;  
  HANDLE  mutex_; 
};

#endif //_INTERPROCESS_MUTEX_H_

