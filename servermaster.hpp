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

#if !defined(_SERVERMASTER_HPP_INCLUDED_)
#define _SERVERMASTER_HPP_INCLUDED_

/*
 * Classes derived from this class allow for the configuration of Internet servers
 * without requiring that the InternetServer class know about what is required to 
 * configure each kind of server that might be implemented.
 */

#include "internetsession.hpp"

class Server;

class ServerMaster {
public:
  ServerMaster(void);
  virtual ~ServerMaster(void) = 0;
  virtual InternetSession *NewSession(SessionDriver *driver, Server *server) = 0;
};

#endif //_SERVERMASTER_HPP_INCLUDED_
