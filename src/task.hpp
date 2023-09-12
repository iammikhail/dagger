#pragma once

#include <concepts>
#include <iostream>
#include <string>
#include <vector>

#include "dag.hpp"
#include "type_utils.hpp"

namespace dagger {
class Task {
  friend class DAG;
  friend class SequentialExecutor;
  friend class NaiveConcurrentExecutor;

  class Node;

  Task(Node *);

  Node *node{nullptr};

 public:
  Task() = default;
  Task(const Task &);

  Task &with_name(const std::string &);

  std::string name();

  template <std::invocable F>
  Task &with_func(F &&);

  void run();

  template <typename... Ts>
    requires Nonempty<Ts...>
  Task &depends_on(Ts &&...);
};

class Task::Node {
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

  Node();

  template <std::invocable F>
  Node(F &&);

  template <std::invocable F>
  Node(const std::string &, F &&);
};

Task::Node::Node() : Node{"", [] {}} {}

template <std::invocable F>
inline Task::Node::Node(F &&func) : Node{"", std::forward<F>(func)} {}

template <std::invocable F>
inline Task::Node::Node(const std::string &name, F &&func)
    : name{name}, func{std::forward<F>(func)} {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

inline Task::Task(const Task &other) : node{other.node} {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

inline Task::Task(Node *node) : node{node} {}

template <typename... Ts>
  requires Nonempty<Ts...>
Task &Task::depends_on(Ts &&...tasks) {
  node->_is_source = false;
  (node->predecessors.push_back(tasks.node), ...);
  (..., tasks.node->successors.push_back(node));
  return *this;
}

inline void Task::run() { node->func(); }

inline Task &Task::with_name(const std::string &name) {
  node->name = name;
  return *this;
}

inline std::string Task::name() { return node->name; }

template <std::invocable F>
Task &Task::with_func(F &&func) {
  node->func = std::function<void()>(std::forward<F>(func));
  return *this;
}

}  // namespace dagger
