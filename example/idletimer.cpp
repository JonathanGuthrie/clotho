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

#include <time.h>

#include <internetserver.hpp>
#include <deltaqueueaction.hpp>

#include "idletimer.hpp"
#include "echomaster.hpp"
#include "echosession.hpp"

IdleTimer::IdleTimer(int delta, InternetSession *session) : DeltaQueueAction(delta, session) {}


void IdleTimer::handleTimeout(bool isPurge) {
  if (!isPurge) {
    EchoSession *session = dynamic_cast<EchoSession *>(m_session);
    const EchoMaster *master = dynamic_cast<const EchoMaster *>(session->master());
    time_t now = time(NULL);
    unsigned timeout = master->idleTimeout();

    if ((now - timeout) > session->lastTrafficTime()) {
      session->idleTimeout();
    }
    else {
      session->server()->addTimerAction(new IdleTimer((time_t) session->lastTrafficTime() + timeout + 1 - now, m_session));
    }
  }
}
