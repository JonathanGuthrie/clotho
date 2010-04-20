#if !defined(_SERVERMASTER_HPP_INCLUDED_)
#define _SERVERMASTER_HPP_INCLUDED_

#include "internetsession.hpp"

class InternetServer;

class ServerMaster {
public:
  ServerMaster(void);
  virtual ~ServerMaster(void) = 0;
  virtual InternetSession *NewSession(SessionDriver *driver, InternetServer *server) = 0;
};

#endif //_SERVERMASTER_HPP_INCLUDED_
