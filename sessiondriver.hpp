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

#if !defined(SESSIONDRIVER_HPP_INCLUDED_)
#define SESSIONDRIVER_HPP_INCLUDED_

#include <string>
#include <stdint.h>
#include <concepts>
#include <coroutine>

#include <boost/thread.hpp>

#include "internetsession.hpp"
#include "insensitive.hpp"
#include "datasource.hpp"

class Server;
class InternetSession;
class ServerMaster;
class Socket;
class SessionDriver;

struct SessionPromise {
    struct promise_type {
      ~promise_type() { }
      SessionPromise get_return_object() {
      return {
          // Uses C++20 designated initializer syntax
          .h_ = std::coroutine_handle<promise_type>::from_promise(*this)
        };
      }
      std::suspend_always initial_suspend() { return {}; }
      std::suspend_always final_suspend() noexcept { return {}; }
      void unhandled_exception() {}
      std::suspend_always yield_value(void) {
        return {};
      }
      void return_void() {}
  };

  auto await_transform(std::string *s);
  std::coroutine_handle<promise_type> h_;
  operator std::coroutine_handle<promise_type>() const { return h_; }
  operator std::coroutine_handle<>() const { return h_; }
  SessionDriver *m_driver;
};

// The SessionDriver class sits between the server, which does the listening
// for more data, and the InternetSession, which does all the processing of the
// data.  It knows server-specific stuff and the merest bit of session stuff.
// This has to be a separate class from InternetServer because I want one of these
// for each InternetSession
class SessionDriver {
public:
  SessionDriver(Server *server, ServerMaster *master);
  ~SessionDriver();
  void doWork(void);
  void newSession(Socket *s);
  void destroySession(void);
  const InternetSession *session(void) const { return m_session; }
  Socket *socket(void) const { return m_sock; }
  Server *server(void) const { return m_server; }
  ServerMaster *master(void) const { return m_master; }
  void lock(void);
  void unlock(void);
  void setUpSend(std::string s) { m_s = s; m_wantsToReceive = false; }
  void setUpReceive(void) { m_wantsToReceive = true; }
  // As soon as the DataSource class's fetch method returns 0, the destructor
  // for the passed pointer will be called
  // void wantsToSend(DataSource *source);
  void startTls(const std::string &keyfile, const std::string &certfile, const std::string &cafile, const std::string &crlfile);
  bool connectionIsEncrypted(void) const;
  std::coroutine_handle<> *coroutineHandle(void) { return &m_coroutineHandle; }
  std::string receivedString(void) { return m_s; }

private:
  Server *m_server;
  Socket *m_sock;
  InternetSession *m_session;
  ServerMaster *m_master;
  boost::mutex *m_workMutex;
  std::coroutine_handle<> m_coroutineHandle;
  bool m_wantsToReceive;
  uint8_t m_buffer[8193];
  std::string m_s;
  void sendData(const std::string &s) const { sendData((uint8_t *)s.data(), s.size()); }
  void sendData(const uint8_t *buffer, size_t length) const;
};


struct SessionSendAwaiter {
  std::coroutine_handle<> *m_coroutineHandle;
  SessionDriver *m_driver;
  std::string m_send;

  bool await_ready() const noexcept { return false; } // says yes call await_suspend
  void await_suspend(std::coroutine_handle<> h) {
    *m_coroutineHandle = h;
    m_driver->setUpSend(std::move(m_send));
  }
  void await_resume(void) const noexcept {};
  SessionSendAwaiter(SessionDriver *driver, std::string s) : m_driver(driver), m_send(s)  { m_coroutineHandle = m_driver->coroutineHandle(); }
};


struct SessionReceiveAwaiter {
  std::coroutine_handle<> *m_coroutineHandle;
  SessionDriver *m_driver;
  std::string m_string;

  bool await_ready() const noexcept { return false; } // says yes call await_suspend
  void await_suspend(std::coroutine_handle<> h) {
    *m_coroutineHandle = h;
    m_driver->setUpReceive();
  }
  std::string await_resume(void) const noexcept { return m_driver->receivedString(); }
  SessionReceiveAwaiter(SessionDriver *driver) : m_driver(driver)  { m_coroutineHandle = m_driver->coroutineHandle(); }
};

#endif // SESSIONDRIVER_HPP_INCLUDED_
