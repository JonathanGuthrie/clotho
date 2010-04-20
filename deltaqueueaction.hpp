#if !defined(_DELTAQUEUEACTION_HPP_INCLUDED_)
#define _DELTAQUEUEACTION_HPP_INCLUDED_

class InternetSession;

class DeltaQueueAction
{
public:
    DeltaQueueAction(int delta, InternetSession *session);
    class DeltaQueueAction *next;
    virtual void HandleTimeout(bool isPurge) = 0;
    unsigned m_delta;
    InternetSession *m_session;
};


#endif // _DELTAQUEUEACTION_HPP_INCLUDED_
