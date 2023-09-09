#include "task.hpp"

void doThing() { std::cout << "Doing a thing!" << std::endl; }

int main() {
  dagger::Task t(doThing);
  t.run();

  dagger::Task t2([] { std::cout << "Doing another thing!" << std::endl; });
  t2.run();

  dagger::Task t3;
  t3.run();
}