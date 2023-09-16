#pragma once

#include <dagger/task.hpp>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace dagger {

class CycleDetected : public std::exception {
  const char *what() const noexcept { return "Invalid graph: cycle detected"; }
};

class DAG {
  using Node = Task::Node;
  std::vector<Node *> nodes;

 public:
  template <typename... Args>
  Task add_task(Args &&...args) {
    nodes.push_back(new Node(std::forward<Args>(args)...));
    return Task(nodes.back());
  }

  inline size_t size() const { return nodes.size(); }

  inline bool empty() const { return nodes.empty(); }
  inline bool empty() { return nodes.empty(); }

  inline std::vector<Task::Node *>::const_iterator begin() const {
    return nodes.cbegin();
  }

  inline std::vector<Task::Node *>::const_iterator end() const {
    return nodes.cend();
  }

  void run(bool dryrun = false);
  ~DAG();
};

}  // namespace dagger
