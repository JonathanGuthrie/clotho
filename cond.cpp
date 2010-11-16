#include "cond.hpp"

#include <iostream>

Cond::Cond() {
  pthread_cond_init(&m_cond, NULL);
}

Cond::~Cond() {
  pthread_cond_destroy(&m_cond);
}

void Cond::Wait(Mutex *lockedMutex) {
  pthread_cond_wait(&m_cond, lockedMutex->mutex());
}

void Cond::Signal() {
  pthread_cond_signal(&m_cond);
}

void Cond::Broadcast() {
  pthread_cond_broadcast(&m_cond);
}
