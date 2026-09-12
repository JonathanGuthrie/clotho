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

#if !defined(INTERNETSESSION_HPP_INCLUDED)
#define INTERNETSESSION_HPP_INCLUDED

/*
 * A derivative of the InternetSession class is instantiated for each connection from a client.
 * Basically, the InternetSession subclass holds all of the state for that connection's
 * operations and the actual protocol implementation is done in derivatives of this class.
 */

#include <stdint.h>
#include <stdlib.h>
#include <coroutine>

class ServerMaster;
class SessionDriver;

struct SessionPromise {
    struct promise_type {
	    enum promiseAction {
    	    wantsToReceive,
	        wantsToSend
	    };
	    promiseAction m_action;
      uint8_t *m_buffer;
	    size_t m_bufferLen;

      ~promise_type() { }
      SessionPromise get_return_object() {
      return {              
          // Uses C++20 designated initializer syntax
          .h_ = std::coroutine_handle<promise_type>::from_promise(*this)
        };
      }
      std::suspend_always initial_suspend() { return {}; }
      std::suspend_never final_suspend() noexcept { return {}; }
      void unhandled_exception() {}
      std::suspend_always yield_value(promiseAction value) {
        m_action = value;
        return {};
      }
      void return_void() {}
  };

  std::coroutine_handle<promise_type> h_;
  operator std::coroutine_handle<promise_type>() const { return h_; }
  operator std::coroutine_handle<>() const { return h_; }
};


struct SessionAwaiter {
  std::coroutine_handle<> *m_coroutineHandle;
  bool await_ready() const noexcept { return false; } // says yes call await_suspend
  void await_suspend(std::coroutine_handle<> h) {
    *m_coroutineHandle = h;
  }
  constexpr void await_resume() const noexcept { }
};

class InternetSession {
public:
  InternetSession(ServerMaster *master, SessionDriver *driver);
  virtual SessionPromise sessionMain(void) = 0;
  virtual ~InternetSession();
  SessionDriver *driver() const { return m_driver; }
  ServerMaster *master() const { return m_master; }

protected:
  ServerMaster *m_master;
  SessionDriver *m_driver;
};

#endif // INTERNETSESSION_HPP_INCLUDED
