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

#include <stdlib.h>

#include "internetsession.hpp"
#include "deltaqueue.hpp"
#include "deltaqueueaction.hpp"
#include "sessiondriver.hpp"

DeltaQueue::DeltaQueue() : m_queueHead(NULL) {
}


// This function is structured kind of strangely, so a word of explanation is in
// order.  This program has multiple threads doing all kinds of things at the same
// time.  In order to prevent race conditions, delta queues are protected by
// a queueMutex.  However, that means that we have to be careful about taking stuff
// out of the queue whether by the timer expiring or by an as yet unsupported
// cancellation.  The obvious way of doing this is to call HandleTimeout in the 
// while loop that checks for those items that have timed out, however this is also
// a wrong way of doing it because HandleTimeout can (and often does) call InsertNewAction
// which also locks queueMutex, leading to deadlock.  One could put an unlock and a lock
// around the call to HandleTimeout in the while loop, but that would be another wrong
// way to do it because the queue might be (and often is) updated during the call to
// HandleTimeout, and the copy used by DeltaQueue might, as a result could be broken.

// The correct way is the way that I do it.  I collect all expired timers into a list and
// then run through the list calling HandleTimeout in a separate while loop after the exit
// from the critical section.  Since this list is local to this function and, therefore, to
// this thread, I don't have to protect it with a mutex, and since I've unlocked the mutex,
// the call to HandleTimeout can do anything it damn well pleases and there won't be a
// deadlock due to the code here.
void DeltaQueue::tick() {
  DeltaQueueAction *temp = NULL;

  m_queueMutex.lock();
  // decrement head
  if (NULL != m_queueHead) {
    --m_queueHead->m_delta;
    if (0 == m_queueHead->m_delta) {
      DeltaQueueAction *endMarker;

      temp = m_queueHead;
      while ((NULL != m_queueHead) && (0 == m_queueHead->m_delta)) {
	endMarker = m_queueHead;
	m_queueHead = m_queueHead->m_next;
      }
      endMarker->m_next = NULL;
    }
  }
  m_queueMutex.unlock();
  while (NULL != temp) {
    DeltaQueueAction *next = temp->m_next;

    temp->m_session->driver()->lock();
    temp->handleTimeout(false);
    temp->m_session->driver()->unlock();
    delete temp;
    temp = next;
  }
}


void DeltaQueue::insertNewAction(DeltaQueueAction *action) {
  m_queueMutex.lock();
  if ((NULL == m_queueHead) || (m_queueHead->m_delta > action->m_delta)) {
    if (NULL != m_queueHead) {
      m_queueHead->m_delta -= action->m_delta;
    }
    action->m_next = m_queueHead;
    m_queueHead = action;
  }
  else {
    // If I get here, I know that the first item is not going to be the new action
    DeltaQueueAction *item = m_queueHead;

    action->m_delta -= m_queueHead->m_delta;
    for (item=m_queueHead; (item->m_next!=NULL) && (item->m_next->m_delta < action->m_delta); item=item->m_next) {
      action->m_delta -= item->m_next->m_delta;
    }
    // When I get here, I know that item points to the item before where the new action goes
    if (NULL != item->m_next) {
      item->m_next->m_delta -= action->m_delta;
    }
    action->m_next = item->m_next;
    item->m_next = action;
  }
  m_queueMutex.unlock();
}


void DeltaQueue::purgeSession(const SessionDriver *driver) {
  DeltaQueueAction *temp, *next;
  DeltaQueueAction *prev = NULL;
  DeltaQueueAction *purgeList = NULL;

  m_queueMutex.lock();
  for(temp = m_queueHead; NULL != temp; temp=next) {
    next = temp->m_next;
    if (driver == temp->m_session->driver()) {
      if (NULL != next) {
	next->m_delta += temp->m_delta;
      }
      if (NULL != prev) {
	prev->m_next = temp->m_next;
      }
      else {
	m_queueHead = temp->m_next;
      }
      temp->m_next = purgeList;
      purgeList = temp;
    }
    else {
      prev = temp;
    }
  }
  m_queueMutex.unlock();
  while (NULL != purgeList) {
    DeltaQueueAction *next = purgeList->m_next;

    purgeList->handleTimeout(true);
    delete purgeList;
    purgeList = next;
  }
}
