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

#if !defined(_SERVER_HPP_INCLUDED_)
#define _SERVER_HPP_INCLUDED_

/*
 * The InternetServer class is kind of the core of the C++ server library.  It opens a socket
 * to listen for incoming connections, creates sessions as needed to handle incoming connections,
 * provides for asynchronous actions for each session, destroys sessions when they need to be
 * destroyed, and in general does the heavy lifting for managing Internet servers in a 
 * multithreaded environment.
 *
 * The overall scheme works like this:  The ServerMaster knows about the configuration items 
 * for a given server type.  This class is instantiated once for each listening socket.  One
 * SessionDriver is instantiated per session, and knows about operating in a threaded environment
 * and how to hand off the work to a InternetSession derivative instance.  That allows the 
 * InternetSession to operate in complete ignorance of how threading, synchronization, and
 * socket I/O are * done, so long as it doesn't peek under the covers, and the InternetServer class
 * doesn't have to know anything about the protocol being implemented by the server.
 *
 * Asynchronous actions for sessions are implemented using a delta queue whose operations are
 * requested through the server.
 */

#include <string>
#include <set>

#include <stdint.h>
#include <netinet/in.h>

#include "deltaqueueaction.hpp"

class SessionDriver;
class ServerMaster;
class DeltaQueue;
class Socket;
class InternetSession;

template <typename T>class ThreadPool;
typedef ThreadPool<SessionDriver *> WorkerPool;

class ServerErrorException {
public:
  ServerErrorException(int error) : m_systemError(error) {}
private:
  int m_systemError;
};

class Server {
public:
  Server(void);
  virtual ~Server();
  virtual void run(void) = 0;
  virtual void shutdown() = 0;
  virtual void addTimerAction(DeltaQueueAction *action) = 0;
  virtual void wantsToReceive(const Socket *sock, SessionDriver *driver) = 0;
  virtual void killSession(SessionDriver *driver) = 0;
};

#endif // _SERVER_HPP_INCLUDED_
