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

#if !defined(_INTERNETSERVER_HPP_INCLUDED_)
#define _INTERNETSERVER_HPP_INCLUDED_

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
#include <list>

#include <stdint.h>
#include <netinet/in.h>

#include "deltaqueueaction.hpp"
#include "server.hpp"

class SessionDriver;
class ServerMaster;
class DeltaQueue;
class Socket;
class InternetSession;

template <typename T>class ThreadPool;
typedef ThreadPool<SessionDriver *> WorkerPool;

class InternetServer : public Server {
public:
  InternetServer(uint32_t bind_address, short bind_port, ServerMaster *master, int num_worker_threads = 10) throw(ServerErrorException);
  InternetServer(uint32_t bind_address, short bind_port, ServerMaster *master, const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile, int num_worker_threads = 10) throw(ServerErrorException);
  virtual ~InternetServer();
  virtual void run();
  virtual void shutdown();
  virtual void addTimerAction(DeltaQueueAction *action);
  virtual void wantsToReceive(const Socket *sock, SessionDriver *driver);
  virtual void killSession(SessionDriver *driver);

private:
  bool m_isRunning;
  Socket *m_listener;

  static void *listenerThreadFunction(void *);
  static void *receiverThreadFunction(void *);
  static void *timerQueueFunction(void *);
    
  WorkerPool *m_pool;
  pthread_t m_listenerThread, m_receiverThread, m_timerQueueThread;
  std::set<SessionDriver *> m_sessions;
  std::list<SessionDriver *> m_sessionCache;
  int m_epollFd;
  ServerMaster *m_master;
  DeltaQueue *m_timerQueue;
  int m_workerCount;
};

#endif // _INTERNETSERVER_HPP_INCLUDED_
