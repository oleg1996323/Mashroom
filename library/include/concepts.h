#pragma once
#include <ranges>
#include <concepts>

template<typename T>
concept RangeOfStrings  = requires{
    requires std::ranges::range<std::ranges::range_value_t<std::decay_t<T>>>;
    requires std::is_same_v<std::decay_t<std::ranges::range_value_t<
    std::ranges::range_value_t<std::decay_t<T>>>>,
    char>;
};

template<typename T>
concept String  = requires{
    requires std::ranges::range<std::decay_t<T>>;
    requires std::is_same_v<std::ranges::range_value_t<
    std::decay_t<T>>,char>;
};