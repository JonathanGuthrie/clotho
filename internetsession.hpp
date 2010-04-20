#if !defined(_INTERNETSESSION_HPP_INCLUDED_)
#define _INTERNETSESSION_HPP_INCLUDED_

#include <stdint.h>
#include <stdlib.h>

class ServerMaster;
class SessionDriver;

class InternetSession {
public:
  InternetSession(ServerMaster *master, SessionDriver *driver);
  virtual void ReceiveData(uint8_t *buffer, size_t size) = 0;
  virtual ~InternetSession();
  SessionDriver *GetDriver() const { return m_driver; }
  ServerMaster *GetMaster() const { return m_master; }

protected:
  ServerMaster *m_master;
  SessionDriver *m_driver;
};

#endif //_INTERNETSESSION_HPP_INCLUDED_
