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

// #include <iostream>

#include "testserver.hpp"
#include "sessiondriver.hpp"

TestServer::TestServer(ServerMaster *master) throw(ServerErrorException) : m_master(master) {
  m_testSocket = NULL;
}

TestServer::~TestServer() {
}

void TestServer::Run(void) {
}

void TestServer::Test(Socket *s) {
  m_keepRunning = true;
  m_wantsToReceive = false;
  m_actionQueueHead = NULL;
  SessionDriver *driver = new SessionDriver(this, m_master);
  driver->NewSession(s);
  while (m_keepRunning) {
    if (m_wantsToReceive) {
      m_wantsToReceive = false;
      driver->DoWork();
    }
    while (NULL != m_actionQueueHead) {
      DeltaQueueAction *temp;

      temp = m_actionQueueHead;
      m_actionQueueHead = m_actionQueueHead->m_next;
      temp->handleTimeout(false);
      delete temp;
    }
  }
}

void TestServer::Shutdown() {
  m_keepRunning = false;
}

void TestServer::AddTimerAction(DeltaQueueAction *action) {
  // std::cout << "Called AddTimerAction" << std::endl;
  action->m_next = m_actionQueueHead;
  m_actionQueueHead = action;
}

void TestServer::WantsToReceive(const Socket *sock, SessionDriver *driver) {
  // std::cout << "Called WantsToReceive" << std::endl;
  m_wantsToReceive = true;
}

void TestServer::KillSession(SessionDriver *driver) {
  // std::cout << "Called KillSession" << std::endl;
  m_keepRunning = false;
}
