#pragma once

#include <queue>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "task.hpp"

namespace dagger {

class CycleDetected : public std::exception {
  const char *what() const noexcept { return "Invalid graph: cycle detected"; }
};

class DAG {
  using Node = Task::Node;
  std::vector<Node *> nodes;

 public:
  template <typename... Args>
  Task add_task(Args &&...args);
  size_t size() const;
  bool empty() const;
  bool empty();

  std::vector<Node *>::const_iterator begin() const;
  std::vector<Node *>::const_iterator end() const;

  void run();
  ~DAG();
};

template <typename... Args>
inline Task DAG::add_task(Args &&...args) {
  nodes.push_back(new Node(std::forward<Args>(args)...));
  return Task(nodes.back());
}

inline bool DAG::empty() const { return nodes.empty(); }
inline bool DAG::empty() { return nodes.empty(); }

inline std::vector<Task::Node *>::const_iterator DAG::begin() const {
  return nodes.cbegin();
}

inline std::vector<Task::Node *>::const_iterator DAG::end() const {
  return nodes.cend();
}

void DAG::run() {
  if (nodes.empty()) return;
  std::unordered_set<Node *> seen;
  std::queue<Node *> q;
  for (auto &node : nodes) {
    if (!node->is_source) continue;
    q.push(node);
  }

  if (q.empty()) {
    throw CycleDetected();
  }

  while (!q.empty()) {
    Node *node = q.front();
    q.pop();
    if (seen.contains(node)) continue;
    node->func();
    for (auto &child : node->successors) {
      q.push(child);
    }
  }
}

DAG::~DAG() {
  while (!nodes.empty()) {
    delete nodes.back();
    nodes.pop_back();
  }
}

size_t DAG::size() const { return nodes.size(); }

}  // namespace dagger
