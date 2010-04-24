#if !defined(_MUTEX_HPP_INCLUDED_)
#define _MUTEX_HPP_INCLUDED_

#include <stdint.h>
#include <netinet/in.h>

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
