#include "internetsession.hpp"
#include "servermaster.hpp"
#include "sessiondriver.hpp"

InternetSession::InternetSession(ServerMaster *master, SessionDriver *driver) : m_master(master), m_driver(driver) {
}


InternetSession::~InternetSession(void) {
}
