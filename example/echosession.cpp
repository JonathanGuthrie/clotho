#include <iostream>

#include <string.h>
#include <time.h>

#include <internetserver.hpp>
#include <sessiondriver.hpp>

#include "echosession.hpp"
#include "echomaster.hpp"
#include "idletimer.hpp"

EchoSession::EchoSession(EchoMaster *master, SessionDriver *driver) : InternetSession(master, driver) {
  m_master = master;
  m_server = driver->GetServer();
  m_driver->WantsToReceive();
  m_server->AddTimerAction(new IdleTimer(m_master->GetIdleTimeout(), this));
  m_lastTrafficTime = time(NULL);
}

EchoSession::~EchoSession(void) {
}

/*
 * This handles ECHO requests.
 *
 * It simply echoes back what you type at it, a line at a time,
 * unless you type a line that just says "quit" at it, and then it
 * quits, or unless you don't type anything for the timeout period
 */

void EchoSession::ReceiveData(uint8_t *buffer, size_t length) {
  // If we're in the message body, there's nothing to do but accumulate the data and hand it off
  // to the request for processing.  If we're in the message header, then unpack the line, and
  // check it to see if it's significant to the session, and then pass to the request for
  // processing.  If I don't have a request, then I accumulate a line and use it to create a
  // request.
  m_driver->WantsToSend(buffer, length);
  if (0 == strncmp((const char *)buffer, "quit\r\n", length)) {
    m_driver->GetServer()->KillSession(m_driver);
  }
  else {
    m_lastTrafficTime = time(NULL);
    m_driver->WantsToReceive();
  }
}

void EchoSession::IdleTimeout(void) {
  m_driver->WantsToSend("It's been too long.  Bye\r\n");
  m_driver->GetServer()->KillSession(m_driver);
}