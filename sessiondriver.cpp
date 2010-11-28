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

#include "sessiondriver.hpp"
#include "server.hpp"
#include "servermaster.hpp"
#include "socket.hpp"
#include "mutex.hpp"

SessionDriver::SessionDriver(Server *server, ServerMaster *master) : m_server(server), m_master(master) {
  m_sock = NULL;
  m_session = NULL;
  m_workMutex = new Mutex();
}


SessionDriver::~SessionDriver(void) {
  if (NULL != m_sock) {
    delete m_sock;
  }
  if (NULL != m_session) {
    delete m_session;
  }
  delete m_workMutex;
}


void SessionDriver::DoWork(void) {
  uint8_t recvBuffer[1000];
  // When it gets here, it knows that the receive on Sock will not block
  ssize_t numOctets = m_sock->Receive(recvBuffer, 1000);
  if (0 < numOctets) {
    Lock();
    m_session->receiveData(recvBuffer, numOctets);
    Unlock();
  }
}


void SessionDriver::DestroySession(void) {
  delete m_session;
  m_session = NULL;
  delete m_sock;
  m_sock = NULL;
}


void SessionDriver::NewSession(Socket *s) {
  m_sock = s;
  m_session = m_master->NewSession(this, m_server);
}


void SessionDriver::WantsToReceive(void) {
  m_server->wantsToReceive(m_sock, this);
}

void SessionDriver::WantsToSend(const uint8_t *buffer, size_t length) const {
  m_sock->Send(buffer, length);
}

void SessionDriver::Lock(void) {
  m_workMutex->lock();
}

void SessionDriver::Unlock(void) {
  m_workMutex->unlock();
}
