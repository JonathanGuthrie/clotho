#if !defined(_HTTPMASTER_HPP_INCLUDED_)
#define _HTTPMASTER_HPP_INCLUDED_

#include "servermaster.hpp"
#include "httpfactory.hpp"

class HttpMaster : public ServerMaster {
public:
  HttpMaster(void);
  virtual ~HttpMaster(void);
  virtual SessionFactory *GetSessionFactory(void);
  virtual SessionDriver *NewDriver(InternetServer *server, int pipeFd);

private:
  HttpFactory m_factory;
};

#endif //_HTTPMASTER_HPP_INCLUDED_
