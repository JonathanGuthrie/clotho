#include "mutex.hpp"

Mutex::Mutex() {
  pthread_mutex_init(&m_mutex, NULL);
}


Mutex::~Mutex() {
  pthread_mutex_destroy(&m_mutex);
}
