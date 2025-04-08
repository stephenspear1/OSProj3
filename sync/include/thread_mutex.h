// Copyright 2025 CSCE 311
//
// This wrapper class allows the management of a single pthread mutex with RAII behavior.
// It is intended for thread synchronization within a process.
//
#ifndef SYNC_INCLUDE_THREAD_MUTEX_H_
#define SYNC_INCLUDE_THREAD_MUTEX_H_

#include <pthread.h>  // POSIX threads C interface

#include <cassert>  // assert
#include <cstddef>  // std::size_t

class ThreadMutex {  // Store me some place safe, NOT GLOBALLY!
 public:
  ThreadMutex();

  ~ThreadMutex();

  void Lock();

  void Unlock();

  pthread_mutex_t* native_handle();

 private:
  pthread_mutex_t mutex_;

  // Non-copyable, non-movable
  ThreadMutex(const ThreadMutex&) = delete;
  ThreadMutex& operator=(const ThreadMutex&) = delete;
};


class ThreadMutexGuard {  // Use me to lock references to ThreadMutex stored by
 public:                  // threads.
  explicit ThreadMutexGuard(ThreadMutex& mutex);

  ~ThreadMutexGuard();

 private:
  ThreadMutex& mutex_;

  // Non-copyable, non-movable
  ThreadMutexGuard(const ThreadMutexGuard&) = delete;
  ThreadMutexGuard& operator=(const ThreadMutexGuard&) = delete;
};

#endif  // SYNC_INCLUDE_THREAD_MUTEX_H_
