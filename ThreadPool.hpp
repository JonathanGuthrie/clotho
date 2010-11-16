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
 */

/*
 * Notes on a single-input-queue redesign.
 *
 * For a while, now, I've been intended to do a redesign to use a single input queue for all
 * of the worker threads.  The idea is that this would result in the fairest work distribution
 * in the likely case of poorly-behaved worker thread code.
 *
 * Among the assumptions that underlie this implementation is the assumption that we don't
 * need to be "fair" in the distribution of work.  That is, it doesn't matter if the first
 * thread does all the work and none of the other threads do any, it only matters that the
 * work gets done in as efficient a manner as possible.
 *
 * The selection of which worker thread gets the next task must be done as efficiently as
 * possible.  So, I'm going to build a queue of inactive worker threads and a
 * collection of all worker threads and use the queue to manage them.
 *
 * So, how does a worker thread know that there is work to be done?  Well, when a thread is
 * created it should check the list of pending work and take the next one if there is one
 * available.  When the worker thread finishes a task, it should also check to see if any
 * work needs to be done.  Also, when tasks are added to the list of pending tasks, it should
 * wake up tasks as needed to assign available threads to pending tasks.
 *
 * Thinking about it, I need to go through the tasks in detail to make sure that things happen
 * when they're supposed to.  For an idle system, that is a system with at least one entry in
 * the idle queue, the sequence of tasks is this:
 *
 * 1)  Lock the idle queue
 * 2)  Pop the value off the idle queue
 * 3)  Set the appropriate task pointer to the task to run
 * 4)  Signal the now non-idle to go
 * 5)  Unlock the idle queue
 *
 * If the system is not idle, then we need to be careful how we do things.  What we want to avoid
 * is something like this:
 *
 * 1) Master thread locks the idle queue
 * 2) Worker thread finishes, locks the pending queue
 * 3) Worker thread sees empty pending queue
 * 4) Worker thread attempts to lock idle queue (blocks because lock is held by the master thread)
 * 5) Master thread sees idle queue empty
 * 6) master thread locks pending queue -- DEADLOCK (master waiting for pending, worker waiting for idle)
 *
 * Or this
 * 
 * 1) Master thread locks the idle queue
 * 2) Master thread sees idle queue empty
 * 3) Master thread unlocks idle queue
 * 4) Worker thread finishes, locks the pending queue
 * 5) Worker thread sees empty pending queue
 * 6) Worker thread locks idle queue
 * 7) Worker thread adds self to idle queue
 * 8) Worker thread unlocks idle queue
 * 9) master thread locks pending queue
 * 10) master thread adds task to pending queue
 * 11) master thread unlocks pending queue
 *
 * In this case, the master doesn't know the worker became idle and the worker doesn't know that work has
 * become available, so nothing proceeds until some worker thread finishes a task and notices the nonempty
 * pending queue.
 *
 * So, What I should do is this:  I need to lock the pending queue and the idle queue in the same order in
 * both threads.  I don't have to lock both of them, but I need to call one the first one to lock and one
 * the second one to lock and I need to never lock the second one unless I lock the first one, well, first.
 *
 * So, which one is first?  Well, I want to arrange it so that, if possible, I only lock one in the normal
 * case or, failing that, I only lock one under load.  The problem with viewing it that way is that the worker
 * threads are mostly interested in pending and the master thread is mostly interested in idle.  Since they
 * are going to be locked every time, I think that it doesn't matter which one I pick.  Therefore, I pick 
 * pending, then idle.
 */

#include <queue>
#include <list>
#include <iostream>

#include "mutex.hpp"
#include "cond.hpp"
#include "thread.hpp"

// class MessageType must have a method called "void DoWork(void)"
template <typename T>
class ThreadPool
{
public:
  typedef typename std::list<T> Tlist;
  typedef typename std::queue<T, Tlist> Tqueue;
  typedef std::queue<int> IntQueue;
  ThreadPool(int numThreads = 1);
  ~ThreadPool(void);
  void SendMessage(T message);
  void SendMessages(Tqueue &messages);
  void SendMessages(Tlist &messages);

private:
  void WorkerThread(void);
  static void *ThreadFunction(void*);

  Cond m_workersGo;
  bool m_stopRunning;
  int m_workerThreadCount;
  Mutex m_pendingQueueMutex;
  Tqueue m_pendingQueue;
  Thread **m_workerThreads;
};

template <typename T>
void *ThreadPool<T>::ThreadFunction(void *instance_pointer) {
  ThreadPool<T> *self = (ThreadPool<T> *)instance_pointer;
  self->WorkerThread();
  return NULL;
}


template <typename T>
void ThreadPool<T>::WorkerThread(void) {
  // std::cout << "In the worker thread method, I've got a listener for thread " << std::endl;
  while(1) {
    m_pendingQueueMutex.Lock();

    if (m_stopRunning) {
      m_pendingQueueMutex.Unlock();
      break;
    }

    T work = NULL;
    if (!m_pendingQueue.empty()) {
      work = m_pendingQueue.front();
      m_pendingQueue.pop();
      m_pendingQueueMutex.Unlock();
    }
    else {
      work = NULL;
      // The wait unlocks m_pendingQueueMutex, and then locks it again when the condition happens
      m_workersGo.Wait(&m_pendingQueueMutex);
      if (!m_pendingQueue.empty()) {
	work = m_pendingQueue.front();
	m_pendingQueue.pop();
      }
      m_pendingQueueMutex.Unlock();
    }
    if (NULL != work) {
      work->DoWork();
    }
  }
}


template <typename T>
ThreadPool<T>::ThreadPool(int numThreads) {
  m_workerThreadCount = numThreads;
  m_stopRunning = false;

  m_workerThreads = new Thread*[m_workerThreadCount];

  m_pendingQueueMutex.Lock();
  for (int i=0; i<m_workerThreadCount; ++i) {
    m_workerThreads[i] = new Thread(ThreadFunction, this);
  }
  m_pendingQueueMutex.Unlock();
}


template <typename T>
ThreadPool<T>::~ThreadPool(void) {
  m_pendingQueueMutex.Lock();
  
  m_stopRunning = true;
  m_pendingQueueMutex.Unlock();
  m_workersGo.Broadcast();

  // I need to wait for each thread because otherwise I don't know when I can clean up
  // Fortunately, the thread destructor does that.
  for (int i=0; i<m_workerThreadCount; ++i) {
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
void ThreadPool<T>::SendMessage(T message)
{
  m_pendingQueueMutex.Lock();
  m_pendingQueue.push(message);
  m_pendingQueueMutex.Unlock();
  m_workersGo.Signal();
}

/*
 * This takes queue of sessions and passes them to the destination.  If there fewer idle threads than
 * messages, it will allocate work to all the threads that it can and then put the rest on the pending
 * queue, otherwise it will allocate all the messages and put nothing on to the idle queue
 */
template <typename T>
void ThreadPool<T>::SendMessages(Tqueue &messages) {
  m_pendingQueueMutex.Lock();
  while (!messages.empty()) {
    m_pendingQueue.push(messages.front());
    messages.pop();
  }
  m_pendingQueueMutex.Unlock();
  m_workersGo.Broadcast();
}

/*
 * This takes list of sessions and passes them to the destination.  If there fewer idle threads than
 * messages, it will allocate work to all the threads that it can and then put the rest on the pending
 * queue, otherwise it will allocate all the messages and put nothing on to the idle queue
 */
template <typename T>
void ThreadPool<T>::SendMessages(Tlist &messages) {
  m_pendingQueueMutex.Lock();
  
  for (typename ThreadPool<T>::Tlist::iterator i=messages.begin(); i!=messages.end(); ++i) {
    m_pendingQueue.push(*i);
  }
  m_pendingQueueMutex.Unlock();
  m_workersGo.Broadcast();
}


#endif // THREADPOOL_H_INCLUDED
