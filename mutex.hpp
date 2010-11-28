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

#if !defined(_MUTEX_HPP_INCLUDED_)
#define _MUTEX_HPP_INCLUDED_

/*
 * This implements your basic mutex variable.
 */

#include <pthread.h>

class Mutex {
private:
  pthread_mutex_t m_mutex;

public:
  void lock(void) { pthread_mutex_lock(&m_mutex); }
  void unlock(void) { pthread_mutex_unlock(&m_mutex); }
  Mutex();
  ~Mutex();
  pthread_mutex_t *mutex() { return &m_mutex; }
};

#endif // _MUTEX_HPP_INCLUDED_
