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

#if !defined(_DELTAQUEUE_HPP_INCLUDED_)
#define _DELTAQUEUE_HPP_INCLUDED_

/*
 * The DeltaQueue class implements a delta queue for the Internet server.
 *
 * It is used internally by the sever library
 */

#include "deltaqueueaction.hpp"
#include "mutex.hpp"

#include <string>

class SessionDriver;

class DeltaQueue {
public:
  void tick(void);
  DeltaQueue();
  void insertNewAction(DeltaQueueAction *action);
  void purgeSession(const SessionDriver *driver);

private:
  Mutex m_queueMutex;
  DeltaQueueAction *m_queueHead;
};

#endif // _DELTAQUEUE_HPP_INCLUDED_
