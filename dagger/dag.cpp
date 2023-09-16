#include <dagger/dag.hpp>

namespace dagger {

void DAG::run(bool dryrun) {
  if (nodes.empty()) return;
  std::unordered_set<Node *> seen;
  std::queue<Node *> q;
  for (auto &node : nodes) {
    if (!node->is_source) continue;
    q.push(node.get());
  }

  if (q.empty()) {
    throw CycleDetected();
  }

  while (!q.empty()) {
    Node *node = q.front();
    q.pop();
    if (seen.contains(node)) continue;
    if (!dryrun) node->func();
    for (auto &child : node->successors) {
      q.push(child);
    }
  }
}

DAGRun *Node::get_current_dag_run() { return owner->dag_runs.front().get(); }

}  // namespace dagger