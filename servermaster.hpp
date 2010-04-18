#if !defined(_SERVERMASTER_HPP_INCLUDED_)
#define _SERVERMASTER_HPP_INCLUDED_

class SessionDriver;
class InternetServer;

class ServerMaster {
public:
  ServerMaster(void);
  virtual ~ServerMaster(void) = 0;
  virtual SessionDriver *NewDriver(InternetServer *server) = 0;
};

#endif //_SERVERMASTER_HPP_INCLUDED_
