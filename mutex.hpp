#if !defined(_MUTEX_HPP_INCLUDED_)
#define _MUTEX_HPP_INCLUDED_

/*
 * This implements your basic mutex variable.
 */

#include <pthread.h>

class Mutex
{
private:
  pthread_mutex_t m_mutex;

public:
  void Lock(void) { pthread_mutex_lock(&m_mutex); }
  void Unlock(void) { pthread_mutex_unlock(&m_mutex); }
  Mutex();
  ~Mutex();
  pthread_mutex_t *GetMutex() { return &m_mutex; }
};

#endif // _MUTEX_HPP_INCLUDED_
