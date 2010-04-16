#include <string.h>

#include "httpsession.hpp"

HttpSession::HttpSession(Socket *sock, ServerMaster *master, SessionDriver *driver) : InternetSession(sock, master, driver) {
}

HttpSession::~HttpSession(void) {
}

/*
 * This handles HTTP requests.
 *
 * An HTTP request is a request-line followed by zero or more header lines followed by
 * a blank line and then a possibly zero-length message body.
 *
 * A request-line is a single line with no continuations.
 *   It consists of a method, a request URI, and then the HTTP version.
 * Header lines are single lines which may have continuations which have
 * the form of "header-name ':' value"
 * The body is whatever.
 *
 * The way I'm going to work it is like this:  When I get the request line, I will pass it to
 * a request constructor that will return a pointer to an instance of the request class.  Then,
 * I call the request class for each header field, paying attention to the Connection field to
 * determine if the socket should be closed at the end of the read request.  I may have to 
 * pay attention to other fields, of course
 *
 * Anyway, I will parse the header fields as they come in, and then after the header I will pass
 * the body off to the subclass and it will be responsible for handling processing the data.
 */

int HttpSession::ReceiveData(uint8_t *buffer, ssize_t length) {
  int result = 1;

  m_s->Send(buffer, length);
  buffer[length] = '\0';
  if (0 == strcmp((const char *)buffer, "quit\r\n")) {
    result = -1;
  }

  return result;
}

