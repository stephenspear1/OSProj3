// Copyright CSCE 311 SP25
//


#include <bankers_resource_manager.h>

#include <ctime>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>


typedef std::vector<std::size_t> ResourceArray;

class BankersData {
 public:
  BankersData(std::size_t id, std::size_t seed,
      const ResourceArray& max, BankersResourceManager* manager)
      : id_(id), seed_(seed), max_(max), curr_(max.size(), 0), manager_(manager) {
    // empty
  }

  bool Step();

  std::ostream* Extract(std::ostream*);

 private: 
  std::size_t id_;
  std::size_t seed_;
  ResourceArray max_;
  ResourceArray curr_;
  ::BankersResourceManager* manager_;
};


// Expects string to be a set of ints delimited by spaces
//
ResourceArray ExtractResourceArray(const std::string&);


// Argument is BankersData *
//
void* StartRoutine(void * arg) {
  BankersData *data = static_cast<BankersData *>(arg);

  while(data->Step());

  return nullptr;
}


int main(int argc, char* argv[]) {
  if (argc < 4) { 
    std::cerr << "Usage:\n\t"
      << "bankers-data <random seed> \"available\" \"max 1\" \"max n\""
      << std::endl;
    return 1;
  }
  const size_t kRandSeed = std::atoi(argv[1]);

  ResourceArray available = ExtractResourceArray(argv[2]);
  BankersResourceManager manager(available);

  std::vector<BankersData> data;
  for (int i = 0; i < argc - 3; ++i) {
    ResourceArray max = ExtractResourceArray(argv[i + 3]);

    manager.AddMax(max);
    data.push_back(BankersData(i, kRandSeed, max, &manager));
  }


  std::vector<::pthread_t> threads(argc - 3);
  for (int i = 0; i < argc - 3; ++i) {
    ::pthread_create(&threads[i], nullptr, StartRoutine, &data[i]);
  }

  for (::pthread_t thread : threads)
    ::pthread_join(thread, 0);

  return 0;
}


ResourceArray ExtractResourceArray(const std::string& string) {
  std::stringstream ss(string);
  std::string item;
  ResourceArray array;
  while (std::getline(ss, item, ' '))  // default delim is '\n'
    if (!item.empty())
      array.push_back(std::stoi(item));

  return array;
}


bool BankersData::Step() {
  // 1.) Determine need
  // 2.) Select request \subseteq need
  // 3.) Make request, if successful, update curr
  // 4.) If need is met, i.e. curr == max,
  //   4.1) Release all held resources
  //   4.2) Return false
  // 5.) Return true
  ResourceArray need(max_.size());
  for (std::size_t i = 0; i < max_.size(); ++i) {
    need[i] = max_[i] - curr_[i];
  }

  // If no more need, we’re done
  if (std::all_of(need.begin(), need.end(), [](std::size_t n) { return n == 0; })) {
    manager_->Release(id_);
    return false;
  }

  // Generate a random request <= need
  ResourceArray request(max_.size());
  static thread_local std::mt19937 gen(id_ + seed_);

  std::size_t count = 0;  // count number of 0 requests
  for (std::size_t i = 0; i < max_.size(); ++i) {
    std::uniform_int_distribution<std::size_t> dist(0, need[i]);
    request[i] = dist(gen);
    count += request[i] == 0 ? 1 : 0;
  }

  // Only make request if at least one resource is requested
  if (count == max_.size())
    return true;

  if (manager_->Request(id_, request)) {
    // Grant succeeded, update current holdings
    for (std::size_t i = 0; i < max_.size(); ++i)
      curr_[i] += request[i];
  }

  ::timespec ts = {0, 100 * 1000000L};
  ::nanosleep(&ts, nullptr);

  return true;
}
std::ostream* BankersData::Extract(std::ostream* out) {
  *out << "ID: " << id_ << ", Max: { ";
  for (int i : max_)
      *out << i << ' ';
  *out << "}, Curr: { ";
  for (int i : curr_)
    *out << i << ' ';
  *out << '}';
  return out;
}
