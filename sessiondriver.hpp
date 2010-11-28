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

#include "internetsession.hpp"
#include "insensitive.hpp"

class Server;
class InternetSession;
class ServerMaster;
class Socket;
class Mutex;

// The SessionDriver class sits between the server, which does the listening
// for more data, and the InternetSession, which does all the processing of the
// data.  It knows server-specific stuff and the merest bit of session stuff.
// This has to be a separate class from InternetServer because I want one of these
// for each InternetSession
class SessionDriver
{
public:
  SessionDriver(Server *server, ServerMaster *master);
  ~SessionDriver();
  void DoWork(void);
  void NewSession(Socket *s);
  void DestroySession(void);
  const InternetSession *session(void) const { return m_session; }
  Socket *socket(void) const { return m_sock; }
  Server *server(void) const { return m_server; }
  ServerMaster *master(void) const { return m_master; }
  void Lock(void);
  void Unlock(void);
  void WantsToReceive(void);
  void WantsToSend(const std::string &s) const { WantsToSend((uint8_t *)s.data(), s.size()); }
  void WantsToSend(const insensitiveString &s) const { WantsToSend((uint8_t *)s.data(), s.size()); }
  void WantsToSend(const char *buffer, size_t length) const { WantsToSend((uint8_t *)buffer, length); }
  void WantsToSend(const char *buffer) const { WantsToSend((uint8_t *)buffer, strlen(buffer)); }
  void WantsToSend(const uint8_t *buffer, size_t length) const;


private:
  Server *m_server;
  Socket *m_sock;
  InternetSession *m_session;
  ServerMaster *m_master;
  Mutex *m_workMutex;
};

#endif //_SESSIONDRIVER_HPP_INCLUDED_
