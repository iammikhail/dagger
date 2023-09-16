#include <dagger/dag.hpp>
#include <dagger/executor.hpp>
#include <dagger/task.hpp>
#include <iostream>
#include <vector>

void doThing() { std::cerr << "Doing a thing!" << std::endl; }

int main() {
  dagger::DAG dag;
  dagger::Task t = dag.add_task(doThing).with_name("t");
  dagger::Task t2 =
      dag.add_task([] { std::cerr << "Doing another thing!" << std::endl; })
          .with_name("t2");

  dagger::Task t3 = dag.add_task().with_name("t3");

  dagger::Task t4 = dag.add_task().with_name("t4");
  t4.with_name("t4").with_func([] { std::cerr << "Hello from t4\n"; });

  t2.depends_on(t4);
  dagger::ThreadPoolExecutor executor;
  std::future<void> f = executor.execute(dag);
  f.wait();
}