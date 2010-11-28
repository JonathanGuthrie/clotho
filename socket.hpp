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

#if !defined(_SOCKET_HPP_INCLUDED_)
#define _SOCKET_HPP_INCLUDED_

/*
 * Your basic IP socket impelementation
 */

#include <stdint.h>
#include <netinet/in.h>

#include <string>

class SocketSocketErrorException {
};

class SocketBindErrorException {
};


class Socket {
private:
  int sock;
  struct sockaddr_in address; 
  // I need a copy constructor because the "connect" function returns a pointer to a Socket instance
  Socket(int socket, struct sockaddr_in address);

public:
  Socket(uint32_t bind_address, short bind_port, int backlog = 16384) throw(SocketSocketErrorException, SocketBindErrorException);
  Socket *accept(void);
  virtual ~Socket();
  virtual ssize_t receive(uint8_t *buffer, size_t size);
  virtual ssize_t send(const uint8_t *data, size_t length);
  int sockNum() const { return sock; }
};

#endif // _SOCKET_HPP_INCLUDED_
