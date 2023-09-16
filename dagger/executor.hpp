#pragma once

#include <dagger/dag.hpp>
#include <dagger/node.hpp>
#include <future>
#include <queue>
#include <thread>

namespace dagger {

template <typename T>
class ExecutorBase {
 public:
  virtual T execute(DAG &) = 0;
};

class SequentialExecutor : public ExecutorBase<void> {
 public:
  void execute(DAG &dag) override;
};

class ThreadPoolExecutor : public ExecutorBase<std::future<void>> {
  std::queue<Node *> work;
  std::mutex work_mutex;
  std::condition_variable work_cv;
  std::mutex done_mutex;
  std::condition_variable done_cv;
  std::vector<std::thread> threads;
  std::atomic<size_t> nodes_consumed = 0;
  std::atomic<bool> done = false;
  std::atomic<size_t> num_dag_runs;

  bool wait_for_task(Node *&);
  void schedule_epilogue(Node *);
  void schedule_dag(const DAG &);

 public:
  ThreadPoolExecutor(size_t = std::thread::hardware_concurrency());
  // TODO implement callback when DAG finishes
  // TODO support custom failure handling
  std::future<void> execute(DAG &);
  ~ThreadPoolExecutor();
};

}  // namespace dagger
