#include <iostream>
#include <vector>

#include "dag.hpp"
#include "task.hpp"

void doThing() { std::cout << "Doing a thing!" << std::endl; }

int main() {
  dagger::DAG dag;
  dagger::Task t = dag.emplace(doThing);
  t.run();
  dagger::Task t2 =
      dag.emplace([] { std::cout << "Doing another thing!" << std::endl; });

  t2.run();

  dagger::Task t3 = dag.emplace();
  t3.run();

  dagger::Task t4 = dag.emplace();
  t4.with_name("t4").with_func([] { std::cout << "Hello from t4\n"; });
  t4.run();

  t3.depends_on(t2);
}