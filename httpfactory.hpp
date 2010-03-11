#if !defined(_HTTPFACTORY_HPP_INCLUDED_)
#define _HTTPFACTORY_HPP_INCLUDED_

#include "sessionfactory.hpp"

class HttpFactory : public SessionFactory {
public:
  HttpFactory(void);
  virtual ~HttpFactory(void);
  virtual InternetSession *NewSession(Socket *s, ServerMaster *master, SessionDriver *driver);
};

#endif //_HTTPFACTORY_HPP_INCLUDED_
