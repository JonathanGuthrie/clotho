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

#include "socket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>

Socket::Socket(int socket, struct sockaddr_in address) {
  this->sock = socket;
  this->address = address;
}

Socket::Socket(uint32_t bind_address, short bind_port, int backlog) throw(SocketSocketErrorException, SocketBindErrorException) {
  try {
    if (0 < (sock = socket(AF_INET, SOCK_STREAM, 0))) {
      int reuse_flag = 1;
      ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(int));
      memset(&address, 0, sizeof(struct sockaddr_in));
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = htonl(bind_address);
      address.sin_port = htons(bind_port);

      if (0 == bind(sock, (struct sockaddr *) &address, sizeof(address))) {
	listen(sock, backlog);
      }
      else {
	throw SocketBindErrorException();
      }
    }
    else {
      throw SocketSocketErrorException();
    }
  }
  catch (SocketBindErrorException) {
    close(sock);
    throw;
  }
}


Socket *Socket::accept(void) {
  struct sockaddr_in temp;
  socklen_t address_len;
  int temp_socket;

  address_len = sizeof(temp);
  temp_socket = ::accept(sock, (struct sockaddr *)&temp, &address_len);
  int reuse_flag = 1;
  return new Socket(temp_socket, temp);
}


Socket::~Socket() {
  close(sock);
}

ssize_t Socket::send(const uint8_t *data, size_t length) {
  return ::send(sock, data, length, 0);
}

ssize_t Socket::receive(uint8_t *buffer, size_t size) {
  return ::recv(sock, buffer, size, 0);
}

