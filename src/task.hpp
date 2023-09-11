#pragma once

#include <string>
#include <vector>

#include "type_utils.hpp"

namespace dagger {
class Task {
  friend class DAG;

  class Node;

  Task(Node *);

  Node *node{nullptr};

 public:
  Task() = default;
  Task(const Task &);

  Task &with_name(const std::string &);

  std::string name();

  template <typename F>
  Task &with_func(F &&);

  void run();

  template <typename... Ts>
    requires Nonempty<Ts...>
  Task &depends_on(Ts &&...);
};

class Task::Node {
  friend class Task;
  friend class DAG;

  std::string name;
  std::function<void()> func;
  std::vector<Node *> successors;
  std::vector<Node *> predecessors;
  bool is_source{false};

  Node();

  template <typename F>
  Node(F &&);
};

Task::Node::Node() : name{""}, func{[] {}} {}

template <typename F>
Task::Node::Node(F &&func) : name{""}, func{std::forward<F>(func)} {}

Task::Task(const Task &other) : node{other.node} {}

Task::Task(Node *node) : node{node} {}

template <typename... Ts>
  requires Nonempty<Ts...>
Task &Task::depends_on(Ts &&...tasks) {
  node->is_source = false;
  (node->predecessors.push_back(tasks.node), ...);
  (..., tasks.node->successors.push_back(node));
  return *this;
}

void Task::run() { node->func(); }

Task &Task::with_name(const std::string &name) {
  node->name = name;
  return *this;
}

std::string Task::name() { return node->name; }

template <typename F>
Task &Task::with_func(F &&func) {
  node->func = std::function<void()>(std::forward<F>(func));
  return *this;
}

}  // namespace dagger
