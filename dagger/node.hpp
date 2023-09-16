#pragma once

#include <concepts>
#include <dagger/dag.hpp>
#include <dagger/dag_run.hpp>
#include <string>
#include <vector>

namespace dagger {

class DAG;

class Node {
  friend class Task;
  friend class DAG;
  friend class ConsumableNode;
  friend class SequentialExecutor;
  friend class ThreadPoolExecutor;
  friend class DAGRun;

  std::string name;
  std::function<void()> func;
  std::vector<Node *> successors;
  std::vector<Node *> predecessors;
  bool is_source{true};
  std::atomic<size_t> consume_cnt{0};
  DAG *owner{nullptr};

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

  DAGRun *get_current_dag_run();
};

}  // namespace dagger
