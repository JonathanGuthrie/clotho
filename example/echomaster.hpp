#if !defined(_ECHOMASTER_HPP_INCLUDED_)
#define _ECHOMASTER_HPP_INCLUDED_

#include <servermaster.hpp>

#include "echosession.hpp"

class EchoMaster : public ServerMaster {
public:
  EchoMaster(int idle_timeout);
  virtual ~EchoMaster(void);
  virtual InternetSession *NewSession(SessionDriver *driver, InternetServer *server);
  int GetIdleTimeout(void) const { return m_idleTimeout; }

private:
  const int m_idleTimeout;
};

#endif //_ECHOMASTER_HPP_INCLUDED_
