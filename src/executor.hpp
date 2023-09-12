#include <future>
#include <queue>
#include <thread>

#include "dag.hpp"

namespace dagger {

class ExecutorBase {
 public:
  virtual void execute(const DAG &) = 0;
};

class SequentialExecutor : public ExecutorBase {
 public:
  void execute(const DAG &dag) override;
};

class NaiveConcurrentExecutor {
  std::queue<Task::Node *> work;
  std::mutex work_mutex;
  std::condition_variable work_cv;
  std::mutex done_mutex;
  std::condition_variable done_cv;
  const DAG *dag{nullptr};
  std::vector<std::thread> threads;
  std::atomic<size_t> nodes_consumed = 0;
  std::promise<void> promise;

  bool wait_for_task(Task::Node *&);

 public:
  NaiveConcurrentExecutor(const DAG &,
                          size_t = std::thread::hardware_concurrency());
  std::future<void> execute();
  ~NaiveConcurrentExecutor();
};

}  // namespace dagger