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

class DeltaQueue
{
public:
    void Tick(void);
    DeltaQueue();
    void InsertNewAction(DeltaQueueAction *action);
    void PurgeSession(const SessionDriver *driver);

private:
    Mutex m_queueMutex;
    DeltaQueueAction *m_queueHead;
};

#endif // _DELTAQUEUE_HPP_INCLUDED_
