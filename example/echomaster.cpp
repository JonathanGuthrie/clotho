#include "echomaster.hpp"

EchoMaster::EchoMaster(int idle_timeout) : m_idleTimeout(idle_timeout) {
}

EchoMaster::~EchoMaster(void) {
}

InternetSession *EchoMaster::NewSession(SessionDriver *driver, InternetServer *server) {
  return new EchoSession(this, driver);
}
