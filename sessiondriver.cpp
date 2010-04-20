#include "sessiondriver.hpp"
#include "internetserver.hpp"
#include "servermaster.hpp"
#include "socket.hpp"

SessionDriver::SessionDriver(InternetServer *server, ServerMaster *master) : m_server(server), m_master(master) {
  m_sock = NULL;
  m_session = NULL;
  pthread_mutex_init(&m_workMutex, NULL);
}


SessionDriver::~SessionDriver(void) {
  if (NULL != m_sock) {
    delete m_sock;
  }
  if (NULL != m_session) {
    delete m_session;
  }
}


void SessionDriver::DoWork(void) {
  uint8_t recvBuffer[1000];
  // When it gets here, it knows that the receive on Sock will not block
  ssize_t numOctets = m_sock->Receive(recvBuffer, 1000);
  if (0 < numOctets) {
    Lock();
    m_session->ReceiveData(recvBuffer, numOctets);
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
  m_server->WantsToReceive(m_sock);
}

void SessionDriver::WantsToSend(const uint8_t *buffer, size_t length) const {
  m_sock->Send(buffer, length);
}
