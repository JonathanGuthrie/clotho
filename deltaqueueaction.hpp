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

#if !defined(_DELTAQUEUEACTION_HPP_INCLUDED_)
#define _DELTAQUEUEACTION_HPP_INCLUDED_

/*
 * DeltaQueueAction is the virtual base classes for delta queues.
 *
 * To cause an asynchronous action, you define a subclass that defines the
 * HandleTimeout operation, and then add objects to the delta queue using the
 * AddTimerAction method in the InternetServer class.
 *
 */

class InternetSession;

class DeltaQueueAction {
public:
  /*
   * delta is the timeout in seconds.  The session pointer points to the InternetSession that needs the
   * asynchronous action.
   */
  DeltaQueueAction(int delta, InternetSession *session);
  class DeltaQueueAction *m_next;
  /*
   * HandleTimeout is where the work happens.  Obviously, you must derive a subclass to do anything with 
   * an asynchronous action.  The isPurge flag will be true if the timer is being called for something
   * other than the timer expiring.  So, if you want to fire of an idle timer, you would do the cleanup
   * and session closing when isPurge is false, otherwise you wouldn't do anything because a true in
   * isPurge means that you're already in the context of a cleanup.
   */
  virtual void handleTimeout(bool isPurge) = 0;
  unsigned m_delta;
  InternetSession *m_session;
};


#endif // _DELTAQUEUEACTION_HPP_INCLUDED_
