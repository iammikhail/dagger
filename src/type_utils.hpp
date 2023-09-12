template <typename... Ts>
concept Nonempty = sizeof...(Ts) > 0;
