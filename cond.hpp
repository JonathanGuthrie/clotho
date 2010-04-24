#if !defined(_COND_HPP_INCLUDED_)
#define _COND_HPP_INCLUDED_

#include <pthread.h>

#include "mutex.hpp"

class Cond
{
private:
  pthread_cond_t m_cond;

public:
  void Wait(Mutex *lockedMutex);
  void Signal();
  Cond();
  ~Cond();
};

#endif // _COND_HPP_INCLUDED_
