#pragma once

#include <future>

namespace dagger {

class DAGRun {
  friend class DAG;
  friend class ThreadPoolExecutor;
  std::promise<void> promise;
  std::atomic<size_t> nodes_to_consume;

  // Cannot copy a DAG run
  DAGRun(const DAGRun &) = delete;
  DAGRun() = default;

  inline DAGRun(size_t n) : nodes_to_consume{n} {}
};

}  // namespace dagger
