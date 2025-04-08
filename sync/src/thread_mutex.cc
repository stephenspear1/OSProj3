// Copyright 2025 CSCE 311
//

#include <thread_mutex.h>


ThreadMutex::ThreadMutex() {
  pthread_mutex_init(&mutex_, nullptr);
}

ThreadMutex::~ThreadMutex() {
  pthread_mutex_destroy(&mutex_);
}

void ThreadMutex::Lock() {
  pthread_mutex_lock(&mutex_);
}

void ThreadMutex::Unlock() {
  pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* ThreadMutex::native_handle() {
  return &mutex_;
}


ThreadMutexGuard::ThreadMutexGuard(ThreadMutex& mutex) : mutex_(mutex) {
  mutex_.Lock();
}

ThreadMutexGuard::~ThreadMutexGuard() {
  mutex_.Unlock();
}


// PAY NO ATTENTION TO THE CODE BEHIND THE CURTAIN
//
// pthread_mutexattr_t attr;
// pthread_mutexattr_init(&attr);
// pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
//
// pthread_mutex_init(&mutex, &attr);
//
// COSTS
//   * Track the owner thread more explicitly
//   * Detect abnormal termination (e.g., SIGKILL, segfault)
//   * Wake up waiting threads and signal that the mutex is now inconsistent
