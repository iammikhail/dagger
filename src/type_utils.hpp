#include <concepts>

template <typename... Ts>
concept Nonempty = sizeof...(Ts) > 0;

template <typename T>
concept Callable = requires(T f) {
  { f() };
};