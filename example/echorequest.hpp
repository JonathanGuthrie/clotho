#if !defined(_ECHOREQUEST_HPP_INCLUDED_)
#define _ECHOREQUEST_HPP_INCLUDED_

#include <string>

#include <stdint.h>

class EchoSession;

class EchoRequest {
private:
  std::string m_method;
  std::string m_uri;
  std::string m_version;
  EchoSession *m_session;
  
public:
  EchoRequest(uint8_t *buffer, size_t length);
};

#endif //_ECHOREQUEST_HPP_INCLUDED_
