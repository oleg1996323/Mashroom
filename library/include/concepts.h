#pragma once
#include <ranges>
#include <concepts>
#include <memory>

template<typename T>
concept numeric_types_concept = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>;

template<typename T>
concept common_types_concept = numeric_types_concept<T> || std::is_class_v<T>;

template<typename T>
concept pair_concept = requires(const T&){
    typename std::decay_t<T>::first_type;
    typename std::decay_t<T>::second_type;
    requires std::is_same_v<std::pair<typename std::decay_t<T>::first_type,
    typename std::decay_t<T>::second_type>, std::remove_cv_t<std::decay_t<T>>>;
};

/**
 * @details Serialization for weak pointer is not implemented due to uncertainty in assignment to shared pointer
 */
template<typename T>
concept smart_pointer_concept = 
std::is_same_v<T, std::unique_ptr<typename T::element_type, typename T::deleter_type>> ||
std::is_same_v<T, std::shared_ptr<typename T::element_type>>;

template<typename T>
concept time_point_concept = requires (const T& time){
    time.time_since_epoch().count();
};

template<typename T>
concept duration_concept = requires (const T& duration){
    duration.count();
};

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