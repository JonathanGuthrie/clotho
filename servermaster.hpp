#if !defined(_SERVERMASTER_HPP_INCLUDED_)
#define _SERVERMASTER_HPP_INCLUDED_

/*
 * Classes derived from this class allow for the configuration of Internet servers
 * without requiring that the InternetServer class know about what is required to 
 * configure each kind of server that might be implemented.
 */

#include "internetsession.hpp"

class InternetServer;

class ServerMaster {
public:
  ServerMaster(void);
  virtual ~ServerMaster(void) = 0;
  virtual InternetSession *NewSession(SessionDriver *driver, InternetServer *server) = 0;
};

#endif //_SERVERMASTER_HPP_INCLUDED_
