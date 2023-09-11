#pragma once

#include <vector>

#include "task.hpp"

namespace dagger {
class DAG {
  using Node = Task::Node;
  std::vector<Node *> nodes;

 public:
  template <typename... Args>
  Task emplace(Args &&...args);
  ~DAG();
};

template <typename... Args>
Task DAG::emplace(Args &&...args) {
  nodes.push_back(new Node(std::forward<Args>(args)...));
  return Task(nodes.back());
}

DAG::~DAG() {
  while (!nodes.empty()) {
    delete nodes.back();
    nodes.pop_back();
  }
}

}  // namespace dagger
