#pragma once

#include <concepts>
#include <dagger/node.hpp>
#include <dagger/type_utils.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace dagger {
class Task {
  friend class DAG;
  friend class ConsumableNode;
  friend class SequentialExecutor;
  friend class ThreadPoolExecutor;

  Node *node{nullptr};

  inline Task(Node *node) : node{node} {}

 public:
  Task() = default;
  inline Task(const Task &other) : node{other.node} {
#ifdef DEBUG
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
  }

  inline Task &with_name(const std::string &name) {
    node->name = name;
    return *this;
  }

  inline std::string name() { return node->name; }

  inline void run() { node->func(); }

  template <std::invocable F>
  inline Task &with_func(F &&func) {
    node->func = std::function<void()>(std::forward<F>(func));
    return *this;
  }

  template <typename... Ts>
    requires Nonempty<Ts...>
  Task &depends_on(Ts &&...tasks) {
    node->is_source = false;
    (node->predecessors.push_back(tasks.node), ...);
    (..., tasks.node->successors.push_back(node));
    return *this;
  }
};

}  // namespace dagger
