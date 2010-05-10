#if !defined(_IDLETIMER_HPP_INCLUDED_)
#define _IDLETIMER_HPP_INCLUDED_

#include <deltaqueueaction.hpp>
#include <internetsession.hpp>

// The idle timer is never reset.  Instead, I keep the time the last command
// was executed and then check for the timeout period elapsing when the timer expires
// and set the timeout for the time since that command happened.
class IdleTimer : public DeltaQueueAction
{
public:
    IdleTimer(int delta, InternetSession *session);
    virtual void HandleTimeout(bool isPurge);
};


#endif // _IDLETIMER_HPP_INCLUDED_
