#include "httpmaster.hpp"
#include "httpfactory.hpp"
#include "httpdriver.hpp"

HttpMaster::HttpMaster(void) {
}

HttpMaster::~HttpMaster(void) {
}

SessionFactory *HttpMaster::GetSessionFactory(void) {
  return &m_factory;
}

SessionDriver *HttpMaster::NewDriver(InternetServer *server, int pipeFd) {
  return new HttpDriver(server, pipeFd, this);
}
