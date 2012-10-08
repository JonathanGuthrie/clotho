/*
 * Copyright 2010 Jonathan R. Guthrie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_SESSIONDRIVER_HPP_INCLUDED_)
#define _SESSIONDRIVER_HPP_INCLUDED_

#include <string.h>
#include <stdint.h>

#include <boost/thread.hpp>

#include "internetsession.hpp"
#include "insensitive.hpp"
#include "datasource.hpp"

class Server;
class InternetSession;
class ServerMaster;
class Socket;

// The SessionDriver class sits between the server, which does the listening
// for more data, and the InternetSession, which does all the processing of the
// data.  It knows server-specific stuff and the merest bit of session stuff.
// This has to be a separate class from InternetServer because I want one of these
// for each InternetSession
class SessionDriver {
public:
  SessionDriver(Server *server, ServerMaster *master);
  ~SessionDriver();
  void doWork(void);
  void newSession(Socket *s);
  void destroySession(void);
  const InternetSession *session(void) const { return m_session; }
  Socket *socket(void) const { return m_sock; }
  Server *server(void) const { return m_server; }
  ServerMaster *master(void) const { return m_master; }
  void lock(void);
  void unlock(void);
  void wantsToReceive(void);
  void wantsToSend(const std::string &s) const { wantsToSend((uint8_t *)s.data(), s.size()); }
  void wantsToSend(const insensitiveString &s) const { wantsToSend((uint8_t *)s.data(), s.size()); }
  void wantsToSend(const char *buffer, size_t length) const { wantsToSend((uint8_t *)buffer, length); }
  void wantsToSend(const char *buffer) const { wantsToSend((uint8_t *)buffer, strlen(buffer)); }
  void wantsToSend(const uint8_t *buffer, size_t length) const;
  // As soon as the DataSource class's fetch method returns 0, the destructor
  // for the passed pointer will be called
  void wantsToSend(DataSource *source);

private:
  Server *m_server;
  Socket *m_sock;
  InternetSession *m_session;
  ServerMaster *m_master;
  boost::mutex *m_workMutex;
};

#endif //_SESSIONDRIVER_HPP_INCLUDED_
