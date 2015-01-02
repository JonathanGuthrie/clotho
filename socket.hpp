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
#include <string>
#include <netinet/in.h>
#include <gnutls/gnutls.h>

class SocketSocketErrorException {
};

class SocketBindErrorException {
};

class TlsException {
public:
  TlsException(std::string message) : m_tlsError(message) {}
private:
  std::string m_tlsError;
};


class Socket {
private:
  int m_sock;
  bool m_isEncrypted;
  struct sockaddr_in m_address; // Todo:  Convert to IPv6
  // TLS variables
  std::string m_keyfile;
  std::string m_certfile;
  std::string m_cafile;
  std::string m_crlfile;
  gnutls_dh_params_t m_dhParams;
  gnutls_certificate_credentials_t m_x509Credentials;
  gnutls_priority_t m_priorityCache;
  gnutls_session_t m_tlsSession;

  // I need a copy constructor because the "connect" function returns a pointer to a Socket instance
  Socket(int socket, struct sockaddr_in address);
  ssize_t socket_receive(uint8_t *buffer, size_t size);
  ssize_t socket_send(const uint8_t *data, size_t length);
  ssize_t tls_receive(uint8_t *buffer, size_t size);
  ssize_t tls_send(const uint8_t *data, size_t length);
  typedef ssize_t(Socket::*receiver)(uint8_t *buffer, size_t size);
  typedef ssize_t(Socket::*sender)(const uint8_t *buffer, size_t size);
  receiver m_receive;
  sender m_send;

public:
  // For an unencrypted socket
  Socket(uint32_t bind_address, short bind_port, int backlog = 16384) throw(SocketSocketErrorException, SocketBindErrorException);
  // For an SSL socket
  Socket(uint32_t bind_address, short bind_port, const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile, int backlog = 16384) throw(SocketSocketErrorException, SocketBindErrorException);
  Socket *accept(void) throw(TlsException);

  bool startTls(const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile) throw(TlsException);
  
  ~Socket();

  ssize_t receive(uint8_t *buffer, size_t size);
  ssize_t send(const uint8_t *buffer, size_t size);

  int sockNum() const { return m_sock; }
  bool isEncrypted() const { return m_isEncrypted; }
};

#endif // _SOCKET_HPP_INCLUDED_
