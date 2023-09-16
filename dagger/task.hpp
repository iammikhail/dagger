#pragma once

#include <concepts>
#include <dagger/type_utils.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace dagger {
class Task {
  friend class DAG;
  friend class SequentialExecutor;
  friend class NaiveConcurrentExecutor;

  class Node {
    friend class Task;
    friend class DAG;
    friend class SequentialExecutor;
    friend class NaiveConcurrentExecutor;

    std::string name;
    std::function<void()> func;
    std::vector<Node *> successors;
    std::vector<Node *> predecessors;
    bool is_source{true};
    std::atomic<size_t> consume_cnt = 0;

    inline Node() : Node{"", [] {}} {}

    template <std::invocable F>
    inline Node(F &&func) : Node{"", std::forward<F>(func)} {}

    template <std::invocable F>
    inline Node(const std::string &name, F &&func)
        : name{name}, func{std::forward<F>(func)} {
#ifdef DEBUG
      std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }
  };

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
