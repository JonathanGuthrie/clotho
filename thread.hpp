#if !defined(_THREAD_HPP_INCLUDED_)
#define _THREAD_HPP_INCLUDED_

/*
 * Your basic thread implementation
 */
#include <pthread.h>

class ThreadException
{
};

class Thread
{
private:
  pthread_t m_thread;

public:
  Thread(void *ThreadFunction(void*), void *data);
  ~Thread();
};

#endif // _THREAD_HPP_INCLUDED_
