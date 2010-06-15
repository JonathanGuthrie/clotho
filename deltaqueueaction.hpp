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

class DeltaQueueAction
{
public:
  /*
   * delta is the timeout in seconds.  The session pointer points to the InternetSession that needs the
   * asynchronous action.
   */
  DeltaQueueAction(int delta, InternetSession *session);
  class DeltaQueueAction *next;
  /*
   * HandleTimeout is where the work happens.  Obviously, you must derive a subclass to do anything with 
   * an asynchronous action.  The isPurge flag will be true if the timer is being called for something
   * other than the timer expiring.  So, if you want to fire of an idle timer, you would do the cleanup
   * and session closing when isPurge is false, otherwise you wouldn't do anything because a true in
   * isPurge means that you're already in the context of a cleanup.
   */
  virtual void HandleTimeout(bool isPurge) = 0;
  unsigned m_delta;
  InternetSession *m_session;
};


#endif // _DELTAQUEUEACTION_HPP_INCLUDED_
