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

#include <stdint.h>
#include <netinet/in.h>

#include "deltaqueueaction.hpp"

class SessionDriver;
class ServerMaster;
class DeltaQueue;
class Socket;
class Mutex;

template <typename T>class ThreadPool;
typedef ThreadPool<SessionDriver *> WorkerPool;

class ServerErrorException
{
public:
  ServerErrorException(int error) : m_systemError(error) {}
private:
  int m_systemError;
};

class InternetServer {
public:
  InternetServer(uint32_t bind_address, short bind_port, ServerMaster *master, int num_worker_threads = 10) throw(ServerErrorException);
  ~InternetServer();
  void Run();
  void Shutdown();
  void AddTimerAction(DeltaQueueAction *action);
  void WantsToReceive(Socket *sock);
  void KillSession(SessionDriver *driver);

private:
  bool m_isRunning;
  Socket *m_listener;
  // static void *SelectThreadFunction(void *);
  static void *ListenerThreadFunction(void *);
  static void *ReceiverThreadFunction(void *);
  static void *TimerQueueFunction(void *);
    
  WorkerPool *m_pool;
  pthread_t m_listenerThread, m_receiverThread, m_timerQueueThread;
  SessionDriver *m_sessions[FD_SETSIZE];
  fd_set m_masterFdList;
  Mutex *m_masterFdMutex;
  int m_pipeFd[2];
  ServerMaster *m_master;
  DeltaQueue *m_timerQueue;
  int m_workerCount;
};

#endif // _INTERNETSERVER_HPP_INCLUDED_
