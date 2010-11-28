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

#if !defined(_INTERNETSESSION_HPP_INCLUDED_)
#define _INTERNETSESSION_HPP_INCLUDED_

/*
 * A derivative of the InternetSession class is instantiated for each connection from a client.
 * Basically, the InternetSession subclass holds all of the state for that connection's
 * operations and the actual protocol implementation is done in derivatives of this class.
 */

#include <stdint.h>
#include <stdlib.h>

class ServerMaster;
class SessionDriver;

class InternetSession {
public:
  InternetSession(ServerMaster *master, SessionDriver *driver);
  virtual void receiveData(uint8_t *buffer, size_t size) = 0;
  virtual ~InternetSession();
  SessionDriver *driver() const { return m_driver; }
  ServerMaster *master() const { return m_master; }

protected:
  ServerMaster *m_master;
  SessionDriver *m_driver;
};

#endif //_INTERNETSESSION_HPP_INCLUDED_
