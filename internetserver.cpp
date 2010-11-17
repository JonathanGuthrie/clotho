#include <time.h>
#include <errno.h>
#include <sys/epoll.h>

#include "internetserver.hpp"
#include "sessiondriver.hpp"
#include "servermaster.hpp"
#include "deltaqueue.hpp"
#include "socket.hpp"

#include "ThreadPool.hpp"
typedef ThreadPool<SessionDriver *> WorkerPool;

InternetServer::InternetServer(uint32_t bind_address, short bind_port, ServerMaster *master, int num_workers) throw(ServerErrorException) {
  m_timerQueue = new DeltaQueue;
  m_workerCount = num_workers;
  m_master = master;
  m_isRunning = true;
  m_listener = new Socket(bind_address, bind_port);
  // SYZYGY -- Need to do something different with the m_sessions
  for (int i=0; i<FD_SETSIZE; ++i) {
    m_sessions[i] = new SessionDriver(this, m_master);
    // new SessionDriver(this, m_pipeFd[1], master);
  }
  m_epollFd = epoll_create1(EPOLL_CLOEXEC);
}


InternetServer::~InternetServer() {
  delete m_timerQueue;
}


void InternetServer::Run() {
  if (0 == pthread_create(&m_listenerThread, NULL, ListenerThreadFunction, this)) {
    if (0 == pthread_create(&m_receiverThread, NULL, ReceiverThreadFunction, this)) {
      if (0 == pthread_create(&m_timerQueueThread, NULL, TimerQueueFunction, this)) {
	m_pool = new WorkerPool(m_workerCount);
      }
    }
  }
}


void InternetServer::Shutdown() {
  m_isRunning = false;
  pthread_cancel(m_listenerThread);
  pthread_join(m_listenerThread, NULL);
  delete m_listener;
  m_listener = NULL;
  close(m_epollFd);
  pthread_join(m_receiverThread, NULL);
  pthread_join(m_timerQueueThread, NULL);

  for (int i=0; i<FD_SETSIZE; ++i) {
      KillSession(m_sessions[i]);
      delete m_sessions[i];
  }
  delete m_pool;
}


void *InternetServer::ListenerThreadFunction(void *d) {
  InternetServer *t = (InternetServer *)d;
  while(t->m_isRunning) {
    Socket *worker = t->m_listener->Accept();
    // SYZYGY -- Need to do something different with the m_sessions
    if (FD_SETSIZE > worker->SockNum()) {
      struct epoll_event event;
      event.events = 0;
      event.data.ptr = NULL;
      errno = 0;
      epoll_ctl(t->m_epollFd, EPOLL_CTL_ADD, worker->SockNum(), &event);
      t->m_sessions[worker->SockNum()]->NewSession(worker);
    }
    else {
      delete worker;
    }
  }
  return NULL;
}


void *InternetServer::ReceiverThreadFunction(void *d) {
  InternetServer *t = (InternetServer *)d;
  while(t->m_isRunning) {
    struct epoll_event events[100];

    int count = epoll_wait(t->m_epollFd, events, 100, 1000);
    for (int i=0; i<count; ++i) {
      t->m_pool->SendMessage((SessionDriver *)events[i].data.ptr);
    }
  }
  return NULL;
}


void *InternetServer::TimerQueueFunction(void *d) {
  InternetServer *t = (InternetServer *)d;
  while(t->m_isRunning) {
    sleep(1);
    // puts("tick");
    t->m_timerQueue->Tick();
  }
  return NULL;
}


void InternetServer::WantsToReceive(const Socket *sock, SessionDriver *driver) {
  // SYZYGY -- This assumes the session table is indexed by socket number
  struct epoll_event event;
  event.events = EPOLLIN | EPOLLONESHOT;
  event.data.ptr = driver;
  errno = 0;
  epoll_ctl(m_epollFd, EPOLL_CTL_MOD, sock->SockNum(), &event);
}


void InternetServer::KillSession(SessionDriver *driver) {
  m_timerQueue->PurgeSession(driver);
  if (NULL != driver->socket()) {
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, driver->socket()->SockNum(), NULL);
    driver->DestroySession();
  }
}


void InternetServer::AddTimerAction(DeltaQueueAction *action) {
  m_timerQueue->InsertNewAction(action);
}
