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

#if !defined(THREADPOOL_H_INCLUDED)
#define THREADPOOL_H_INCLUDED

/*
 * The template in this file implements a worker thread pool.  The template must be
 * based on a class that implements a method called DoWork that neither accepts nor
 * returns anything.  The class that uses the thread pool sends work to them using
 * the SendMessage and SendMessages methods.
 *
 * In the case of the InternetServer, the thread pools are called to notify them that
 * the socket will not block on receive, and the worker itself is expected to do the
 * actual receiving.
 *
 * Among the assumptions that underlie this implementation is the assumption that we don't
 * need to be "fair" in the distribution of work.  That is, it doesn't matter if the first
 * thread does all the work and none of the other threads do any, it only matters that the
 * work gets done in as efficient a manner as possible.
 *
 */

#include <queue>
#include <list>

#include <boost/thread.hpp>

// class MessageType must have a method called "void DoWork(void)"
template <typename T>
class ThreadPool
{
public:
  typedef typename std::list<T> Tlist;
  typedef typename std::queue<T, Tlist> Tqueue;
  typedef std::queue<int> IntQueue;
  ThreadPool(int numThreads = 1, typename Tqueue::size_type queue_highwater = 0);
  ~ThreadPool(void);
  void sendMessage(T message);
  void sendMessages(Tqueue &messages);
  void sendMessages(Tlist &messages);

private:
  void workerThread(void);
  static void *ThreadFunction(void*);

  boost::condition_variable m_workersGo;
  bool m_stopRunning;
  int m_workerThreadCount;
  boost::mutex m_pendingQueueMutex;
  boost::unique_lock<boost::mutex> m_pendingQueueLock;
  Tqueue m_pendingQueue;
  boost::mutex m_highwaterMutex;
  boost::unique_lock<boost::mutex> m_highwaterLock;
  boost::condition_variable m_highwaterCond;
  boost::thread **m_workerThreads;
  const typename ThreadPool<T>::Tqueue::size_type m_queueHighwater;
};

template <typename T>
void *ThreadPool<T>::ThreadFunction(void *instance_pointer) {
  ThreadPool<T> *self = (ThreadPool<T> *)instance_pointer;
  self->workerThread();
  return NULL;
}


template <typename T>
void ThreadPool<T>::workerThread(void) {
  // std::cout << "In the worker thread method, I've got a listener for thread " << std::endl;
  while(1) {
    m_pendingQueueLock.lock();
    if ((0 < m_queueHighwater) && (m_queueHighwater > m_pendingQueue.size())) {
      m_highwaterCond.notify_all();
    }

    if (m_stopRunning) {
      m_pendingQueueLock.unlock();
      m_workersGo.notify_all();
      break;
    }

    T work = NULL;
    if (!m_pendingQueue.empty()) {
      work = m_pendingQueue.front();
      m_pendingQueue.pop();
      m_pendingQueueLock.unlock();
    }
    else {
      work = NULL;
      // The wait unlocks m_pendingQueueLock, and then locks it again when the condition happens
      m_workersGo.wait(m_pendingQueueLock);
      if (!m_pendingQueue.empty()) {
	work = m_pendingQueue.front();
	m_pendingQueue.pop();
      }
      m_pendingQueueLock.unlock();
    }
    if (NULL != work) {
      work->doWork();
    }
  }
}


template <typename T>
ThreadPool<T>::ThreadPool(int numThreads, typename ThreadPool<T>::Tqueue::size_type queue_highwater) : m_pendingQueueLock(m_pendingQueueMutex),m_highwaterLock(m_highwaterMutex),m_queueHighwater(queue_highwater) {
  m_workerThreadCount = numThreads;
  m_stopRunning = false;

  m_workerThreads = new boost::thread*[m_workerThreadCount];

  for (int i=0; i<m_workerThreadCount; ++i) {
    m_workerThreads[i] = new boost::thread(ThreadFunction, this);
  }
  m_pendingQueueLock.unlock();
}


template <typename T>
ThreadPool<T>::~ThreadPool(void) {
  m_pendingQueueLock.lock();
  
  m_stopRunning = true;
  m_pendingQueueLock.unlock();
  m_workersGo.notify_all();
  m_highwaterCond.notify_all();

  // I need to wait for each thread because otherwise I don't know when I can clean up
  for (int i=0; i<m_workerThreadCount; ++i) {
    m_workerThreads[i]->join();
    delete m_workerThreads[i];
  }
  delete[] m_workerThreads;
}


/*
 * This takes a single session and does the right thing with it.  If there is an idle session, then
 * give it to that session and signal the condition variable.  If there are no idle sessions, then
 * put this at the back of the message queue and go about our way.
 */
template <typename T>
void ThreadPool<T>::sendMessage(T message)
{
  typename ThreadPool<T>::Tqueue::size_type depth;
  
  m_pendingQueueLock.lock();
  m_pendingQueue.push(message);
  depth = m_pendingQueue.size();
  m_pendingQueueLock.unlock();
  m_workersGo.notify_one();
  if ((0 < m_queueHighwater) && (m_queueHighwater < depth)) {
    m_highwaterLock.lock();
    m_highwaterCond.wait(m_highwaterLock);
    m_highwaterLock.unlock();
  }
}

/*
 * This takes queue of sessions and passes them to the destination.  If there fewer idle threads than
 * messages, it will allocate work to all the threads that it can and then put the rest on the pending
 * queue, otherwise it will allocate all the messages and put nothing on to the idle queue
 */
template <typename T>
void ThreadPool<T>::sendMessages(Tqueue &messages) {
  typename ThreadPool<T>::Tqueue::size_type depth;

  m_pendingQueueLock.lock();
  while (!messages.empty()) {
    m_pendingQueue.push(messages.front());
    messages.pop();
  }
  depth = m_pendingQueue.size();
  m_pendingQueueLock.unlock();
  m_workersGo.notify_all();
  if ((0 < m_queueHighwater) && (m_queueHighwater < depth)) {
    m_highwaterLock.lock();
    m_highwaterCond.wait(m_highwaterLock);
    m_highwaterLock.unlock();
  }
}

/*
 * This takes list of sessions and passes them to the destination.  If there fewer idle threads than
 * messages, it will allocate work to all the threads that it can and then put the rest on the pending
 * queue, otherwise it will allocate all the messages and put nothing on to the idle queue
 */
template <typename T>
void ThreadPool<T>::sendMessages(Tlist &messages) {
  typename ThreadPool<T>::Tqueue::size_type depth;
  m_pendingQueueLock.lock();
  
  for (typename ThreadPool<T>::Tlist::iterator i=messages.begin(); i!=messages.end(); ++i) {
    m_pendingQueue.push(*i);
  }
  depth = m_pendingQueue.size();
  m_pendingQueueLock.unlock();
  m_workersGo.notify_all();
  if ((0 < m_queueHighwater) && (m_queueHighwater < depth)) {
    m_highwaterLock.lock();
    m_highwaterCond.wait(m_highwaterLock);
    m_highwaterLock.unlock();
  }
}


#endif // THREADPOOL_H_INCLUDED
