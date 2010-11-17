#include <time.h>

#include <internetserver.hpp>
#include <deltaqueueaction.hpp>

#include "idletimer.hpp"
#include "echomaster.hpp"
#include "echosession.hpp"

IdleTimer::IdleTimer(int delta, InternetSession *session) : DeltaQueueAction(delta, session) {}


void IdleTimer::HandleTimeout(bool isPurge) {
    if (!isPurge) {
        EchoSession *session = dynamic_cast<EchoSession *>(m_session);
        const EchoMaster *master = dynamic_cast<const EchoMaster *>(session->master());
	time_t now = time(NULL);
	unsigned timeout = master->IdleTimeout();

	if ((now - timeout) > session->LastTrafficTime()) {
	    session->IdleTimeout();
	}
	else {
	    session->server()->AddTimerAction(new IdleTimer((time_t) session->LastTrafficTime() + timeout + 1 - now, m_session));
	}
    }
}
