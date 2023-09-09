#pragma once

#include <iostream>
#include <vector>

namespace dagger {
class Task {
  std::string _name;
  std::function<void()> _func;
  std::vector<Task *> _parents;

 public:
  Task();

  template <typename F>
  Task(F &&);

  template <typename F>
  Task(const std::string &, F &&);

  void run();

  std::string name();

  void set_name(const std::string &);

  template <typename... Ts>
  Task &depends_on(Ts &&...tasks);
};

Task::Task() : _name{""}, _func{[] {}} {}

template <typename F>
Task::Task(F &&func) : _name{""}, _func{std::forward<F>(func)} {}

template <typename F>
Task::Task(const std::string &name, F &&func)
    : _name{name}, _func{std::forward<F>(func)} {}

void Task::run() { _func(); }

std::string Task::name() { return _name; }

void Task::set_name(const std::string &name) { _name = name; }

template <typename... Ts>
Task &Task::depends_on(Ts &&...tasks) {
  (_parents.push_back(&tasks), ...);
  return *this;
}
}  // namespace dagger
