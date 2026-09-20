/*
 * Copyright 2025 Jonathan R. Guthrie
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

#include <string>
#include <time.h>

#include <internetserver.hpp>
#include <sessiondriver.hpp>

#include "echosession.hpp"
#include "echomaster.hpp"
#include "idletimer.hpp"
#include "socket.hpp"

EchoSession::EchoSession(EchoMaster *master, SessionDriver *driver) : InternetSession(master, driver) {
  m_master = master;
  m_server = driver->server();
  m_server->addTimerAction(new IdleTimer(m_master->idleTimeout(), this));
  m_lastTrafficTime = time(NULL);
}

std::string EchoSession::prompt(void) const {
   return "Hello User!  Write something to me, and I'll echo it back.\r\n\r\nWrite 'quit' to quit.\r\n";
}

EchoSession::~EchoSession(void) {
}

/*
 * This handles ECHO requests.
 *
 * It simply echoes back what you type at it, a line at a time,
 * unless you type a line that just says "quit" at it, and then it
 * quits, or unless you don't type anything for the timeout period
 */

 SessionPromise EchoSession::sessionMain(void) {
  std::string s;
  do {
    s = co_await SessionReceiveAwaiter{m_driver};
    if (s != "quit\r\n") {
      co_await SessionSendAwaiter{m_driver, s};
      m_lastTrafficTime = time(NULL);
    }
  } while (s != "quit\r\n");
}

void EchoSession::idleTimeout(void) {
  m_driver->killSession("It's been too long.  Bye\r\n");
}
