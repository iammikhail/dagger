#include <iostream>
#include <vector>

#include "dag.hpp"
#include "task.hpp"

void doThing() { std::cout << "Doing a thing!" << std::endl; }

int main() {
  dagger::DAG dag;
  dagger::Task t = dag.add_task(doThing).with_name("t");
  dagger::Task t2 =
      dag.add_task([] { std::cout << "Doing another thing!" << std::endl; })
          .with_name("t2");

  dagger::Task t3 = dag.add_task().with_name("t3");

  dagger::Task t4 = dag.add_task().with_name("t4");
  t4.with_name("t4").with_func([] { std::cout << "Hello from t4\n"; });

  t2.depends_on(t4);
  dag.run();
}