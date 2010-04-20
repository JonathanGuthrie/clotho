#if !defined(_SESSIONDRIVER_HPP_INCLUDED_)
#define _SESSIONDRIVER_HPP_INCLUDED_

#include <pthread.h>

#include "socket.hpp"
#include "internetsession.hpp"

class InternetServer;
class InternetSession;
class ServerMaster;

// The SessionDriver class sits between the server, which does the listening
// for more data, and the InternetSession, which does all the processing of the
// data.  It knows server-specific stuff and the merest bit of session stuff.
// This has to be a separate class from InternetServer because I want one of these
// for each InternetSession
class SessionDriver
{
public:
  SessionDriver(InternetServer *server, ServerMaster *master);
  ~SessionDriver();
  void DoWork(void);
  void NewSession(Socket *s);
  void DestroySession(void);
  const InternetSession *GetSession(void) const { return m_session; }
  Socket *GetSocket(void) const { return m_sock; }
  InternetServer *GetServer(void) const { return m_server; }
  ServerMaster *GetMaster(void) const { return m_master; }
  void Lock(void) { pthread_mutex_lock(&m_workMutex); }
  void Unlock(void) { pthread_mutex_unlock(&m_workMutex); }


private:
  InternetServer *m_server;
  Socket *m_sock;
  InternetSession *m_session;
  ServerMaster *m_master;
  pthread_mutex_t m_workMutex;
};

#endif //_SESSIONDRIVER_HPP_INCLUDED_
