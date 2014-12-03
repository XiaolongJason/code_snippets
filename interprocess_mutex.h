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
		mutex_.lock();
  }

  ~ScopedLock()
  {
		mutex_.unlock();
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
		SECURITY_DESCRIPTOR security_descriptor;
		::InitializeSecurityDescriptor(&security_descriptor, SECURITY_DESCRIPTOR_REVISION);
		::SetSecurityDescriptorDacl(&security_descriptor,TRUE,NULL,FALSE);
		SECURITY_ATTRIBUTES security_attributes;
		security_attributes.nLength = sizeof SECURITY_ATTRIBUTES;
		security_attributes.bInheritHandle = FALSE;
		security_attributes.lpSecurityDescriptor = &security_descriptor;
		mutex_ = CreateMutex(&security_attributes, FALSE, name_.c_str());
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
	  mutex_ = NULL;
    }
  }

	void lock()
  {
    if (mutex_)
    {
      WaitForSingleObject(mutex_, INFINITE);
    }
  }
  
	void unlock()
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

