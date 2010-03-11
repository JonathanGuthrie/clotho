#include "httpfactory.hpp"
#include "httpsession.hpp"

HttpFactory::HttpFactory(void) {
}

HttpFactory::~HttpFactory(void) {
}

InternetSession *HttpFactory::NewSession(Socket *s, ServerMaster *master, SessionDriver *driver) {
  return new HttpSession(s, master, driver);
}
