#include "internetserver.hpp"
#include "servermaster.hpp"

#include "httpdriver.hpp"
#include "httpsession.hpp"

HttpDriver::HttpDriver(InternetServer *s, ServerMaster *master) : SessionDriver (s, master) {
  m_sock = NULL;
  m_session = NULL;
  pthread_mutex_init(&m_workMutex, NULL);
}

HttpDriver::~HttpDriver(void) {
  if (NULL != m_sock) {
    delete m_sock;
  }
  if (NULL != m_session) {
    delete m_session;
  }
}

void HttpDriver::DoWork(void) {
  // When I get here, I know that the receive will not block
  uint8_t recvBuffer[1000];
  // When it gets here, it knows that the receive on Sock will not block
  ssize_t numOctets = m_sock->Receive(recvBuffer, 1000);
  if (0 < numOctets) {
    HttpSession *http_session = dynamic_cast<HttpSession *>(m_session);
    pthread_mutex_lock(&m_workMutex);
    int result = http_session->ReceiveData(recvBuffer, numOctets);
    pthread_mutex_unlock(&m_workMutex);
    if (0 > result) {
      m_server->KillSession(this);
    }
    else {
      m_server->WantsToReceive(m_sock->SockNum());
    }
  }
}

void HttpDriver::NewSession(Socket *s) {
  m_sock = s;
  m_session = m_master->GetSessionFactory()->NewSession(s, m_master, this);
  m_server->WantsToReceive(m_sock->SockNum());
}

void HttpDriver::DestroySession(void) {
  delete m_sock;
  m_sock = NULL;
  delete m_session;
  m_session = NULL;
}
