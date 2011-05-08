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

#if !defined(_TESTSERVER_HPP_INCLUDED_)
#define _TESTSERVER_HPP_INCLUDED_

/*
 * The InternetServer class is kind of the core of the C++ server library, and the TestServer
 * class is kind of a little brother to the InternetServer class.  I discovered that I needed
 * such a thing because I wanted to drive an InternetSession derivative from a test program, so
 * I did not need any thread pools, and I needed to be able to specify a socket-like thing to
 * feed data to the session and to accept the response, and this is the result.
 */

#include <list>

#include <stdint.h>
#include <netinet/in.h>

#include "server.hpp"
#include "deltaqueueaction.hpp"

class SessionDriver;
class ServerMaster;
class Socket;
class InternetSession;

class TestServer : public Server {
public:
  typedef std::list<DeltaQueueAction> ActionQueue;
  TestServer(ServerMaster *master) throw(ServerErrorException);
  virtual ~TestServer();
  virtual void run(void);
  virtual void test(Socket *s);
  virtual void shutdown();
  virtual void addTimerAction(DeltaQueueAction *action);
  virtual void wantsToReceive(const Socket *sock, SessionDriver *driver);
  virtual void killSession(SessionDriver *driver);
  void runTimer(void);
  bool queueEmpty(void) const { return NULL == m_actionQueueHead; }

private:
  Socket *m_testSocket;
  ServerMaster *m_master;
  bool m_keepRunning;
  bool m_wantsToReceive;
  DeltaQueueAction *m_actionQueueHead;
  SessionDriver *m_driver;
};

#endif // _TESTSERVER_HPP_INCLUDED_
