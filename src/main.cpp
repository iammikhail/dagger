#include "task.hpp"

void doThing() { std::cout << "Doing a thing!" << std::endl; }

int main() {
  tasky::Task t(doThing);
  t.run();

  tasky::Task t2([] { std::cout << "Doing another thing!" << std::endl; });
  t2.run();

  tasky::Task t3;
  t3.run();
}