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
  delete m_driver;
}

void TestServer::run(void) {
}

void TestServer::test(Socket *s) {
  m_keepRunning = true;
  m_wantsToReceive = false;
  m_actionQueueHead = NULL;
  m_driver = new SessionDriver(this, m_master);
  m_driver->newSession(s);
  while (m_keepRunning) {
    m_keepRunning = false;
    if (m_wantsToReceive) {
      m_keepRunning = true;
      m_wantsToReceive = false;
      m_driver->doWork();
    }
    // If it doesn't want to receive more data, then it's waiting for a timer to expire
    // so run through the timer items until it does want to receive more data
    while (!m_wantsToReceive) {
      runTimer();
    }
  }
}

void TestServer::shutdown() {
  m_keepRunning = false;
}

void TestServer::addTimerAction(DeltaQueueAction *action) {
  // std::cout << "Called AddTimerAction" << std::endl;
  action->m_next = m_actionQueueHead;
  m_actionQueueHead = action;
}

void TestServer::wantsToReceive(const Socket *sock, SessionDriver *driver) {
  // std::cout << "Called WantsToReceive" << std::endl;
  m_wantsToReceive = true;
}

void TestServer::killSession(SessionDriver *driver) {
  // std::cout << "Called KillSession" << std::endl;
  m_keepRunning = false;
}

void TestServer::runTimer(void) {
  // This runs all the actions once
  DeltaQueueAction *p;

  p = m_actionQueueHead;
  m_actionQueueHead = NULL;
  while (NULL != p) {
    m_keepRunning = true;
    DeltaQueueAction *temp;

    temp = p;
    p = p->m_next;
    temp->handleTimeout(false);
    delete temp;
  }
}
