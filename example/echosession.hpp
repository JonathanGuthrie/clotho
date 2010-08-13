#if !defined(_ECHOSESSION_HPP_INCLUDED_)
#define _ECHOSESSION_HPP_INCLUDED_

#include <internetsession.hpp>

class EchoMaster;
class InternetServer;

class EchoSession :  public InternetSession {
private:
  InternetServer *m_server;
  EchoMaster *m_master;
  time_t m_lastTrafficTime;
 
public:
  EchoSession(EchoMaster *master, SessionDriver *driver);
  virtual ~EchoSession();
  virtual void ReceiveData(uint8_t *buffer, size_t length);
  time_t GetLastTrafficTime(void) const { return m_lastTrafficTime; }
  InternetServer *GetServer(void) const { return m_server; }
  void IdleTimeout(void);
  
};

#endif //_ECHOSESSION_HPP_INCLUDED_
