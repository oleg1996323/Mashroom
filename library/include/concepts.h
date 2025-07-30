#pragma once
#include <ranges>
#include <concepts>
#include <memory>
#include <chrono>

template<typename T>
concept numeric_types_concept = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>;

template<typename T>
concept common_types_concept = numeric_types_concept<T> || std::is_class_v<T>;

template<typename T>
concept pair_concept = requires(const T& p) {
    { p.first } -> std::convertible_to<const typename T::first_type&>;
    { p.second } -> std::convertible_to<const typename T::second_type&>;
};

static_assert(pair_concept<std::pair<int,int>>);
/**
 * @details Serialization for weak pointer is not implemented due to uncertainty in assignment to shared pointer
 */
template<typename T>
concept smart_pointer_concept = 
requires(T ptr) {
    typename T::element_type;
    { ptr.get() } -> std::convertible_to<typename T::element_type*>;
    { bool(ptr) } -> std::convertible_to<bool>;
};
static_assert(smart_pointer_concept<std::shared_ptr<int>>);
static_assert(smart_pointer_concept<std::unique_ptr<int>>);

template<typename T>
concept time_point_concept = requires (const T& time){
    time.time_since_epoch().count();
};
static_assert(time_point_concept<std::chrono::system_clock::time_point>);
template<typename T>
concept duration_concept = requires (const T& duration){
    duration.count();
};
static_assert(duration_concept<std::chrono::system_clock::duration>);
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

template<typename T>
concept AssociativeContainer = requires(const T& cont){
    typename std::decay_t<T>::key_type;
    typename std::decay_t<T>::mapped_type;
    requires std::ranges::range<std::decay_t<T>>;
};

template<typename T>
inline constexpr bool is_associative_container_v = AssociativeContainer<T>;