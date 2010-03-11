#include <string.h>

#include "httpsession.hpp"

HttpSession::HttpSession(Socket *sock, ServerMaster *master, SessionDriver *driver) : InternetSession(sock, master, driver) {
  sock->Send((const uint8_t *)"Hello, world!\r\n", 13);
}

HttpSession::~HttpSession(void) {
}

int HttpSession::ReceiveData(uint8_t *buffer, ssize_t length) {
  int result = 1;

  m_s->Send(buffer, length);
  buffer[length] = '\0';
  if (0 == strcmp((const char *)buffer, "quit\r\n")) {
    result = -1;
  }

  return result;
}

