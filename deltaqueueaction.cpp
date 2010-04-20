#include <stdlib.h>

#include "deltaqueueaction.hpp"

DeltaQueueAction::DeltaQueueAction(int delta, InternetSession *session) : next(NULL), m_delta(delta), m_session(session)
{
}


