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

#if !defined(_ECHOSESSION_HPP_INCLUDED_)
#define _ECHOSESSION_HPP_INCLUDED_

#include <internetsession.hpp>

class EchoMaster;
class InternetServer;

class EchoSession :  public InternetSession {
private:
  Server *m_server;
  EchoMaster *m_master;
  time_t m_lastTrafficTime;
 
public:
  EchoSession(EchoMaster *master, SessionDriver *driver);
  virtual ~EchoSession();
  virtual void receiveData(uint8_t *buffer, size_t length);
  time_t lastTrafficTime(void) const { return m_lastTrafficTime; }
  Server *server(void) const { return m_server; }
  void idleTimeout(void);
};

#endif //_ECHOSESSION_HPP_INCLUDED_
