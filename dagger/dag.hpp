#pragma once

#include <dagger/dag_run.hpp>
#include <dagger/node.hpp>
#include <dagger/task.hpp>
#include <memory>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dagger {

class CycleDetected : public std::exception {
  const char *what() const noexcept { return "Invalid graph: cycle detected"; }
};

class DAG {
  friend class Node;
  friend class ThreadPoolExecutor;
  std::vector<std::unique_ptr<Node>> nodes;
  std::queue<std::unique_ptr<DAGRun>> dag_runs;

  typedef std::vector<std::unique_ptr<Node>> NodeVector;

 public:
  template <typename... Args>
  Task add_task(Args &&...args) {
    nodes.push_back(
        std::unique_ptr<Node>(new Node(std::forward<Args>(args)...)));
    nodes.back()->owner = this;
    return Task(nodes.back().get());
  }

  inline size_t size() const { return nodes.size(); }

  inline bool empty() const { return nodes.empty(); }
  inline bool empty() { return nodes.empty(); }

  inline NodeVector::const_iterator begin() const { return nodes.cbegin(); }

  inline NodeVector::const_iterator end() const { return nodes.cend(); }

  void run(bool dryrun = false);

  DAGRun &create_run() {
    dag_runs.push(std::unique_ptr<DAGRun>(new DAGRun(nodes.size())));
    return *dag_runs.back();
  }
};

}  // namespace dagger
