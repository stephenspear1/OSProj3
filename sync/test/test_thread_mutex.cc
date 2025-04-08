// Copyright CSCE 311 SP25
// 
// This simple test creates n threads, has them use ThreadMutex to manage a
// critical section. The threads created with odd indices should increament the
// counter. Even indices will give up MUTEX and exit without modifying the
// counter.

#include <thread_mutex.h>  // Include mutex class header
#include <pthread.h>
#include <iostream>
#include <vector>
#include <unistd.h>  // For sleep (simulating work)


// Shared resource
struct SharedResource {
  int *shared_counter;
  ::ThreadMutex *global_mutex;  // Mutex that will be shared among threads
  ::pthread_t id;
  std::size_t index;
};


void* ThreadFunction(void* arg) {
  SharedResource* shared_res = static_cast<SharedResource *>(arg);
  bool some_condition = shared_res->index % 2 == 0;
  
  std::cout << "Thread " << shared_res->index << " attempting to enter critical section...\n";
  
  ::ThreadMutexGuard guard(*(shared_res->global_mutex));  // Automatically locks and unlocks
  
  if (some_condition) {
      std::cout << "Thread " << shared_res->index << " exiting early.\n";
  
      return nullptr;  // Mutex is unlocked here!
  }
  
  // Simulated work
  ++*shared_res->shared_counter;
  std::cout << "Thread " << shared_res->index << " incremented counter to "
    << *shared_res->shared_counter << ".\n";
  sleep(1);  // Simulate work
  
  return nullptr;  // Mutex is unlocked on function exit as well
}

int main() {
    constexpr int NUM_THREADS = 6;
    std::vector<SharedResource> threads(NUM_THREADS);

    std::cout << "Starting test with " << NUM_THREADS << " threads...\n";

    // Create shared resource
    int counter = 0;
    ::ThreadMutex mutex;
    // Create threads
    for (std::size_t i = 0; i < NUM_THREADS; ++i) {
      threads[i] = {&counter, &mutex, ::pthread_t(), i};

      pthread_create(&(threads[i].id), nullptr, ThreadFunction, &threads[i]);
    }

    // Wait for threads to finish
    for (auto& thread : threads) {
        pthread_join(thread.id, nullptr);
    }

    std::cout << "Final shared_counter value: " << counter << "\n";
    return 0;
}

