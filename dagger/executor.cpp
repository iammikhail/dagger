#include <dagger/executor.hpp>
#include <queue>
#include <unordered_set>

namespace dagger {
void SequentialExecutor::execute(const DAG &dag) {
  if (dag.empty()) return;
  std::unordered_set<Task::Node *> seen;
  std::queue<Task::Node *> q;
  for (const auto &node : dag) {
    if (!node->is_source) continue;
    q.push(node);
  }

  if (q.empty()) {
    throw CycleDetected();
  }

  while (!q.empty()) {
    Task::Node *node = q.front();
    q.pop();
    if (seen.contains(node)) continue;
    node->func();
    for (auto &child : node->successors) {
      q.push(child);
    }
  }
}

bool NaiveConcurrentExecutor::wait_for_task(Task::Node *&node) {
  std::unique_lock lock(work_mutex);
  work_cv.wait(lock, [&]() { return !work.empty() || done; });
  if (done) {
    return false;
  }
  node = work.front();
  work.pop();
  return true;
}

NaiveConcurrentExecutor::NaiveConcurrentExecutor(const DAG &dag,
                                                 size_t num_threads)
    : dag{&dag}, threads{num_threads} {
  for (size_t i = 0; i < num_threads; ++i) {
    threads[i] = std::thread([this, i = i]() {
      while (!done) {
        Task::Node *node{nullptr};
        if (!wait_for_task(node)) {
          return;
        }

        // thread now has a valid node to run
        node->func();

        if (node->successors.size() > 0) {
          std::scoped_lock lock(work_mutex);
          for (auto &child : node->successors) {
            if (child->consume_cnt.fetch_sub(1) == 1) work.push(child);
          }
          work_cv.notify_one();
        }

        if (nodes_consumed.fetch_add(1) == this->dag->size() - 1) {
          promise.set_value();
          done = true;
          done_cv.notify_one();
          work_cv.notify_all();
          return;
        }
      }
    });
  }
}

std::future<void> NaiveConcurrentExecutor::execute() {
  for (auto &node : *dag) {
    if (node->is_source) {
      work.push(node);
    } else {
      node->consume_cnt.store(node->predecessors.size());
    }
    work_cv.notify_all();
  }
  return promise.get_future();
}

NaiveConcurrentExecutor::~NaiveConcurrentExecutor() {
  std::unique_lock lock(done_mutex);
  done_cv.wait(lock, [&]() { return done.load(); });

  for (auto &t : threads) {
    t.join();
  }
}

}  // namespace dagger
