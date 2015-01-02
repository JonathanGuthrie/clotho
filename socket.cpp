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
#include <unistd.h>
#include <string.h>

Socket::Socket(int socket, struct sockaddr_in address)  {
  m_tlsSession = NULL;
  m_x509Credentials = NULL;
  m_priorityCache = NULL;
  m_dhParams = NULL;
  this->m_sock = socket;
  this->m_address = address;
  this->m_isEncrypted = false;
  this->m_receive = &Socket::socket_receive;
  this->m_send = &Socket::socket_send;
}

Socket::Socket(uint32_t bind_address, short bind_port, int backlog) throw(SocketSocketErrorException, SocketBindErrorException) {
  m_tlsSession = NULL;
  m_x509Credentials = NULL;
  m_priorityCache = NULL;
  m_dhParams = NULL;
  try {
    if (0 < (m_sock = socket(AF_INET, SOCK_STREAM, 0))) {
      int reuse_flag = 1;
      ::setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(int));
      memset(&m_address, 0, sizeof(struct sockaddr_in));
      m_address.sin_family = AF_INET;
      m_address.sin_addr.s_addr = htonl(bind_address);
      m_address.sin_port = htons(bind_port);

      if (0 == bind(m_sock, (struct sockaddr *) &m_address, sizeof(m_address))) {
	listen(m_sock, backlog);
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
    close(m_sock);
    throw;
  }
  this->m_isEncrypted = false;
}


Socket::Socket(uint32_t bind_address, short bind_port, const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile, int backlog) throw(SocketSocketErrorException, SocketBindErrorException) {
  m_keyfile = keyfile;
  m_certfile = certfile;
  m_cafile = cafile;
  m_crlfile = crlfile;
  m_tlsSession = NULL;
  m_x509Credentials = NULL;
  m_priorityCache = NULL;
  m_dhParams = NULL;

  try {
    if (0 < (m_sock = socket(AF_INET, SOCK_STREAM, 0))) {
      int reuse_flag = 1;
      ::setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(int));
      memset(&m_address, 0, sizeof(struct sockaddr_in));
      m_address.sin_family = AF_INET;
      m_address.sin_addr.s_addr = htonl(bind_address);
      m_address.sin_port = htons(bind_port);

      if (0 == bind(m_sock, (struct sockaddr *) &m_address, sizeof(m_address))) {
	listen(m_sock, backlog);
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
    close(m_sock);
    throw;
  }
  this->m_isEncrypted = true;
}


Socket *Socket::accept(void) throw(TlsException) {
  struct sockaddr_in temp;
  socklen_t address_len;
  int temp_socket;

  address_len = sizeof(temp);
  temp_socket = ::accept(m_sock, (struct sockaddr *)&temp, &address_len);
  int reuse_flag = 1;
  (void) reuse_flag;
  Socket *result = new Socket(temp_socket, temp);
  if (m_isEncrypted) {
    result->startTls(m_keyfile, m_certfile, m_cafile, m_crlfile);
  }
  return result;
}


Socket::~Socket() {
  if (NULL != m_tlsSession) {
    gnutls_bye(m_tlsSession, GNUTLS_SHUT_WR);
  }
  if (NULL != m_tlsSession) {
    gnutls_deinit(m_tlsSession);
  }
  close(m_sock);
  if (NULL != m_x509Credentials) {
    gnutls_certificate_free_credentials(m_x509Credentials);
  }
  if (NULL != m_priorityCache) {
    gnutls_priority_deinit(m_priorityCache);
  }
  gnutls_global_deinit();
}

ssize_t Socket::send(const uint8_t *data, size_t length) {
  return (this->*m_send)(data, length);
}

ssize_t Socket::receive(uint8_t *buffer, size_t size) {
  return (this->*m_receive)(buffer, size);
}

ssize_t Socket::socket_send(const uint8_t *data, size_t length) {
  return ::send(m_sock, data, length, 0);
}

ssize_t Socket::socket_receive(uint8_t *buffer, size_t size) {
  return ::recv(m_sock, buffer, size, 0);
}

ssize_t Socket::tls_send(const uint8_t *data, size_t length) {
  return ::gnutls_record_send(m_tlsSession, data, length);
}

ssize_t Socket::tls_receive(uint8_t *buffer, size_t size) {
  int ret;

  do {
    ret = ::gnutls_record_recv(m_tlsSession, buffer, size);
  } while ((0 > ret) && (0 == gnutls_error_is_fatal(ret)));
  return ret;
}

bool Socket::startTls(const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile) throw(TlsException) {
  m_keyfile = keyfile;
  m_certfile = certfile;
  m_cafile = cafile;
  m_crlfile = crlfile;

  gnutls_global_init();

  gnutls_certificate_allocate_credentials(&m_x509Credentials);
  gnutls_certificate_set_x509_trust_file(m_x509Credentials, m_cafile.c_str(), GNUTLS_X509_FMT_PEM);
  gnutls_certificate_set_x509_crl_file(m_x509Credentials, m_crlfile.c_str(), GNUTLS_X509_FMT_PEM);
  if (0 > gnutls_certificate_set_x509_key_file(m_x509Credentials, m_certfile.c_str(), m_keyfile.c_str(), GNUTLS_X509_FMT_PEM)) {
    throw TlsException("Unable to open key file");
  }

  gnutls_dh_params_init(&m_dhParams);
  gnutls_dh_params_generate2(m_dhParams, 1024);

  gnutls_priority_init(&m_priorityCache, "NORMAL", NULL);
  gnutls_certificate_set_dh_params(m_x509Credentials, m_dhParams);

  gnutls_init(&m_tlsSession, GNUTLS_SERVER);
  gnutls_priority_set(m_tlsSession, m_priorityCache);
  gnutls_credentials_set(m_tlsSession, GNUTLS_CRD_CERTIFICATE, m_x509Credentials);

  gnutls_certificate_server_set_request(m_tlsSession, GNUTLS_CERT_IGNORE);
  gnutls_transport_set_ptr(m_tlsSession, (gnutls_transport_ptr_t) m_sock);

  int ret;
  do {
    ret = gnutls_handshake(m_tlsSession);
  } while ((0 > ret) && (0 == gnutls_error_is_fatal(ret)));

  if (ret < 0) {
    gnutls_deinit(m_tlsSession);
    throw TlsException("TLS Handshake Failed");
  }

  // At this point, we're sending and receiving TLS
  m_receive = &Socket::tls_receive;
  m_send = &Socket::tls_send;

  this->m_isEncrypted = true;
  return ret;
}
