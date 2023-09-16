#include <dagger/executor.hpp>
#include <dagger/node.hpp>
#include <queue>
#include <unordered_set>

namespace dagger {

void SequentialExecutor::execute(DAG &dag) {
  if (dag.empty()) return;
  std::unordered_set<Node *> seen;
  std::queue<Node *> q;
  for (const auto &node : dag) {
    if (!node->is_source) {
      continue;
    }
    q.push(node.get());
  }

  if (q.empty()) {
    throw CycleDetected();
  }

  while (!q.empty()) {
    Node *node = q.front();
    q.pop();
    if (seen.contains(node)) {
      continue;
    }
    node->func();
    for (auto &child : node->successors) {
      q.push(child);
    }
  }
}

// Returns false if there are no more tasks to wait for
bool ThreadPoolExecutor::wait_for_task(Node *&node) {
  std::unique_lock lock(work_mutex);
  work_cv.wait(lock, [&]() { return !work.empty() || done; });
  if (done) {
    return false;
  }
  node = work.front();
  work.pop();
  return true;
}

void ThreadPoolExecutor::schedule_epilogue(Node *node) {
  auto dag_run = node->get_current_dag_run();
  if (dag_run->nodes_to_consume.fetch_sub(1) != 1) {
    return;
  }
  node->owner->dag_runs.pop();
  dag_run->promise.set_value();

  // if there are more times the dag should run
  if (node->owner->dag_runs.size() > 0) {
    schedule_dag(*node->owner);
  } else if (num_dag_runs.fetch_sub(1) == 1) {
    // Signal to destructor thread for OK to destruct
    done_cv.notify_one();
  }
}

ThreadPoolExecutor::ThreadPoolExecutor(size_t num_threads)
    : threads{num_threads} {
  for (size_t i = 0; i < num_threads; ++i) {
    threads[i] = std::thread([this]() {
      while (!done) {
        Node *node{nullptr};
        if (!wait_for_task(node)) {
          continue;
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

        schedule_epilogue(node);
      }
    });
  }
}

void ThreadPoolExecutor::schedule_dag(const DAG &dag) {
  for (auto &node : dag) {
    if (node->is_source) {
      std::scoped_lock lock(work_mutex);
      work.push(node.get());
    } else {
      node->consume_cnt.store(node->predecessors.size());
    }
  }
  work_cv.notify_all();
}

std::future<void> ThreadPoolExecutor::execute(DAG &dag) {
  DAGRun &run = dag.create_run();
  ++num_dag_runs;
  auto future = run.promise.get_future();
  if (dag.dag_runs.size() > 1) {
    return future;
  }
  schedule_dag(dag);
  return future;
}

ThreadPoolExecutor::~ThreadPoolExecutor() {
  std::unique_lock lock(done_mutex);
  done_cv.wait(lock, [&]() { return num_dag_runs.load() == 0; });
  // signal that we are done to worker threads so they can finish executing
  done.store(true);
  work_cv.notify_all();
  for (auto &t : threads) {
    t.join();
  }
}

}  // namespace dagger
