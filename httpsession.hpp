#if !defined(_HTTPSESSION_HPP_INCLUDED_)
#define _HTTPSESSION_HPP_INCLUDED_

#include "internetsession.hpp"

class HttpSession :  public InternetSession {
public:
  HttpSession(Socket *sock, ServerMaster *master, SessionDriver *driver);
  virtual ~HttpSession();
  int ReceiveData(uint8_t *buffer, ssize_t length);
};

#endif //_HTTPSESSION_HPP_INCLUDED_
