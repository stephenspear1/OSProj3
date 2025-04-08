#include <bankers_resource_manager.h>

#include <iostream>
#include <algorithm>
#include <stdexcept>

// Adding maximum resources a thread can request
void BankersResourceManager::AddMax(const std::vector<std::size_t>& max) {
    if (max.empty()) {
        return;
    }

    max_.push_back(max);
    allocation_.push_back(std::vector<std::size_t>(max.size(), 0));  // Initialize with 0
    need_.push_back(max);  // Initially, need is the same as max

}

// Constructor: Initializes the available resources
BankersResourceManager::BankersResourceManager(const std::vector<std::size_t>& available)
    : available_(available) {
    // Ensure the available resources list is not empty
    if (available_.size() == 0) {
        std::cerr << "Error: Available resources are empty\n";
    }

    for (std::size_t i = 0; i < available_.size(); ++i) {
        std::cout << available_[i] << " ";
    }
    std::cout << "}\n";
}

// Release resources held by a thread
void BankersResourceManager::Release(std::size_t tIndex) {
    ThreadMutexGuard guard(mutex_);

    // Check if the thread holds any resources to release
    if (std::all_of(allocation_[tIndex].begin(), allocation_[tIndex].end(), [](std::size_t x) { return x == 0; })) {
        std::cerr << "Thread " << tIndex << " has no resources to release.\n";
        return;
    }

    // Release the resources held by the thread
    std::cout << "Thread " << tIndex << " has released: { ";
    for (std::size_t i = 0; i < allocation_[tIndex].size(); ++i) {
        available_[i] += allocation_[tIndex][i];
        std::cout << allocation_[tIndex][i] << " ";
        allocation_[tIndex][i] = 0;  // Set allocation to 0 after release
    }
    std::cout << "}\n";
}

// Requesting resources from the manager
bool BankersResourceManager::Request(std::size_t tIndex, const std::vector<std::size_t>& request) {
    ThreadMutexGuard guard(mutex_);

    // Calculate the need for the thread
    for (std::size_t i = 0; i < max_[tIndex].size(); ++i) {
        need_[tIndex][i] = max_[tIndex][i] - allocation_[tIndex][i];
    }

    // Print the request details
    std::cout << "Thread " << tIndex << " requested: { ";
    for (const auto& r : request) {
       std::cout << r << " "; 
    }
    std::cout << "}\n";

    std::cout << "\tNeed: { ";
    for (const auto& n : need_[tIndex]) {
       std::cout << n << " "; 
    }
    std::cout << "}\n";

    std::cout << "\tAvailable: { ";
    for (const auto& a : available_) {
        std::cout << a << " ";
    }
    std::cout << "}\n";

    // Step 1: Validate the request
    for (std::size_t i = 0; i < request.size(); ++i) {
        if (request[i] > need_[tIndex][i]) {
            return false;
        }
        if (request[i] > available_[i]) {
            return false;
        }
    }

    for (std::size_t i = 0; i < request.size(); ++i) {
        available_[i] -= request[i];
        allocation_[tIndex][i] += request[i];
        need_[tIndex][i] -= request[i];
    }

    // Step 3: Safety check with correct safe execution order
    std::vector<bool> finish(max_.size(), false);
    std::vector<std::size_t> work = available_;
    std::vector<std::size_t> safeOrder;  // To track the order of threads that can finish

    bool found;
    while (true) {
        found = false;
        for (std::size_t i = 0; i < max_.size(); ++i) {
            if (!finish[i]) {
                bool canFinish = true;
                for (std::size_t j = 0; j < need_[i].size(); ++j) {
                    if (need_[i][j] > work[j]) {
                        canFinish = false;
                        break;
                    }
                }

                if (canFinish) {
                    // Mark thread as finished
                    finish[i] = true;
                    safeOrder.push_back(i);  // Add this thread to the safe execution order
                    // Release resources back to the available pool
                    for (std::size_t j = 0; j < work.size(); ++j) {
                        work[j] += allocation_[i][j];
                    }
                    found = true;
                    break;
                }
            }
        }

        // If no progress was made, break out of the loop (deadlock detected)
        if (!found) {
            std::cout << "\tNot Available, request denied.\n";

            // Rollback the changes if not safe
            for (std::size_t i = 0; i < request.size(); ++i) {
                available_[i] += request[i];
                allocation_[tIndex][i] -= request[i];
                need_[tIndex][i] += request[i];
            }

            return false;
        }
        
        if (std::all_of(finish.begin(), finish.end(), [](bool f) { return f; })) {
            break;
        }
    }

    // If safe, print the safe order
    std::cout << "\tSafe. Request allocated. Order: { ";
    for (const auto& threadId : safeOrder) {
        std::cout << "P" << threadId << " ";
    }
    std::cout << "}\n";
        std::cout << "\tNeed: { ";
    for (const auto& n : need_[tIndex]) std::cout << n << " ";
    std::cout << "}\n";

    std::cout << "\tAvailable: { ";
    for (const auto& a : available_) std::cout << a << " ";
    std::cout << "}\n";
    // If any thread cannot finish, it's unsafe
    for (const auto& f : finish) {
        if (!f) {
            std::cerr << "\tNot Available, request denied.\n";

            // Rollback the allocation if unsafe
            for (std::size_t i = 0; i < request.size(); ++i) {
                available_[i] += request[i];
                allocation_[tIndex][i] -= request[i];
                need_[tIndex][i] += request[i];
            }

            return false;
        }
    }

    return true;
}