#if !defined(_HTTPDRIVER_HPP_INCLUDED_)
#define _HTTPDRIVER_HPP_INCLUDED_

#include "sessiondriver.hpp"

class HttpDriver : public SessionDriver {
public:
  HttpDriver(InternetServer *s, int pipe, ServerMaster *master);
  virtual ~HttpDriver();
  virtual void DoWork(void);
  virtual void NewSession(Socket *s);
  virtual void DestroySession(void);

private:
  pthread_mutex_t m_workMutex;
};

#endif //_HTTPDRIVER_HPP_INCLUDED_
