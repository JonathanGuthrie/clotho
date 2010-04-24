#include "thread.hpp"

Thread::Thread(void *function(void*), void *data) {
  if (0 != pthread_create(&m_thread, NULL, function, data)) {
    throw ThreadException();
  }
}


Thread::~Thread() {
  pthread_join(m_thread, NULL);
}
