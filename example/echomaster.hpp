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

#if !defined(_ECHOMASTER_HPP_INCLUDED_)
#define _ECHOMASTER_HPP_INCLUDED_

#include <servermaster.hpp>

#include "echosession.hpp"

class EchoMaster : public ServerMaster {
public:
  EchoMaster(int idle_timeout);
  virtual ~EchoMaster(void);
  virtual InternetSession *newSession(SessionDriver *driver, Server *server);
  int idleTimeout(void) const { return m_idleTimeout; }

private:
  const int m_idleTimeout;
};

#endif //_ECHOMASTER_HPP_INCLUDED_
