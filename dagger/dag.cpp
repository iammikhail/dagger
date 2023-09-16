#include <dagger/dag.hpp>

namespace dagger {

void DAG::run(bool dryrun) {
  if (nodes.empty()) return;
  std::unordered_set<Node *> seen;
  std::queue<Node *> q;
  for (auto &node : nodes) {
    if (!node->is_source) continue;
    q.push(node);
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

DAG::~DAG() {
  while (!nodes.empty()) {
    delete nodes.back();
    nodes.pop_back();
  }
}

}  // namespace dagger