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

SessionDriver::SessionDriver(Server *server, ServerMaster *master) : m_server(server), m_sock(NULL), m_session(NULL), m_master(master) {
  m_workMutex = new boost::mutex();
}


SessionDriver::~SessionDriver(void) {
  delete m_sock;
  m_sock = NULL;
  delete m_session;
  m_session = NULL;
  delete m_workMutex;
  m_workMutex = NULL;
}


void SessionDriver::doWork(void) {
    lock();
    m_coroutineHandle();  //SYZYGY  -- the m_session->sessionMain() is part of the m_coroutineHandle
    unlock();
}


void SessionDriver::destroySession(void) {
  delete m_session;
  m_session = NULL;
  delete m_sock;
  m_sock = NULL;
}


void SessionDriver::newSession(Socket *s) {
  m_sock = s;
  m_session = m_master->newSession(this, m_server);
  m_session->sessionMain(&m_coroutineHandle);
  m_server->wantsToSend(m_sock, this);
}


void SessionDriver::sendData(const uint8_t *buffer, size_t length) const {
    // TODO:  Set this up to do an asynchronous write, and start up again when it's done
    m_sock->send(buffer, length);
}

// TODO:  What the heck should this receive?  Look to the boost socket library
// Okay, it passes some mutable buffers and returns a size_t.  We'll get around to that
void SessionDriver::receiveData(uint8_t *buffer, size_t size) const {
}

void SessionDriver::startTls(const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile) {
  try {
    m_sock->startTls(keyfile, certfile, cafile, crlfile);
  } catch(TlsException e) {
    // TODO:  Log the exception
  }
}

bool SessionDriver::connectionIsEncrypted(void) const {
  return m_sock->isEncrypted();
}

#if 0
void SessionDriver::wantsToSend(DataSource *source) {
  m_source = source;
  // I need to think about how to implement this.  Each socket seems to need a
  // buffer, but I don't necessarily get who manages that buffer.
  m_sock->send()
  // SYZYGY
}
#endif // 0

void SessionDriver::lock(void) {
  if (NULL != m_workMutex) {
    m_workMutex->lock();
  }
}

void SessionDriver::unlock(void) {
  if (NULL != m_workMutex) {
    m_workMutex->unlock();
  }
}
