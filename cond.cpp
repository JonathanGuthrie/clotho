/*
 * Copyright 2010 Jonathan R. Guthrie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cond.hpp"

#include <iostream>

Cond::Cond() {
  pthread_cond_init(&m_cond, NULL);
}

Cond::~Cond() {
  pthread_cond_destroy(&m_cond);
}

void Cond::wait(Mutex *lockedMutex) {
  pthread_cond_wait(&m_cond, lockedMutex->mutex());
}

void Cond::signal() {
  pthread_cond_signal(&m_cond);
}

void Cond::broadcast() {
  pthread_cond_broadcast(&m_cond);
}
