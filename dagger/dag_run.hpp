#pragma once

#include <future>

namespace dagger {

class DAGRun {
  friend class DAG;
  friend class ThreadPoolExecutor;
  std::promise<void> promise;
  std::atomic<size_t> nodes_to_consume;

  //   inline DAGRun(DAG *dag) : nodes_to_consume{dag->nodes.size()} {}
  // Cannot copy a DAG run
  DAGRun(const DAGRun &) = delete;
  DAGRun() = default;

 public:
  inline DAGRun(size_t n) : nodes_to_consume{n} {}
};

}  // namespace dagger
