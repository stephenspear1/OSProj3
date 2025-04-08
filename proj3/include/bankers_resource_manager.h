// Copyright 2025 Stephen Spear
#ifndef BANKERS_RESOURCE_MANAGER_H
#define BANKERS_RESOURCE_MANAGER_H

#include <thread_mutex.h>

#include <vector>
#include <algorithm>
#include <iostream>


class BankersResourceManager {
 public:
  BankersResourceManager(const std::vector<std::size_t>& available);

  void AddMax(const std::vector<std::size_t>& max);

  bool Request(std::size_t threadIndex, const std::vector<std::size_t>& request);

  void Release(std::size_t threadIndex);

 private:
  std::vector<std::size_t> available_;
  std::vector<std::vector<std::size_t>> max_;
  std::vector<std::vector<std::size_t>> allocation_;
  std::vector<std::vector<std::size_t>> need_;
  ThreadMutex mutex_;
};

#endif // BANKERS_RESOURCE_MANAGER_H