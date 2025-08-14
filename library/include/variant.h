#pragma once
#include <variant>

#define ENABLE_DERIVED_VARIANT(Derived, BaseVariant)\
    template<>\
    struct std::variant_size<Derived>\
          : std::variant_size<BaseVariant> {};\
    template<std::size_t I>\
    struct std::variant_alternative<I, Derived>\
          : std::variant_alternative<I, BaseVariant> {} 


template<typename VARIANT,typename = void>
struct VariantFactory: std::false_type{};

template<template<typename... Types> typename VARIANT, typename... Types>
requires IsStdVariant<VARIANT<Types...>>
struct VariantFactory<VARIANT<Types...>>{
      using VariantType = VARIANT<Types...>;
      using ResultType = VariantType;

      template<typename... ARGS>
      static bool emplace(ResultType& result, size_t index,ARGS&&... args) {
      if (index >= sizeof...(Types))
            return false;
      return emplaceImpl(result, index, std::make_index_sequence<sizeof...(Types)>{},std::forward<ARGS>(args)...);
      }
private:
      template <size_t... Is,typename... ARGS>
      static bool emplaceImpl(ResultType& result, size_t index, std::index_sequence<Is...>,ARGS&&... args) noexcept{
            bool err = false;
            auto try_emplace = [&]<size_t ID> requires (std::is_constructible_v<std::variant_alternative_t<ID, VariantType>,ARGS...> || 
                                                      std::is_default_constructible_v<std::variant_alternative_t<ID, VariantType>>)()
            {
                  using Type = std::variant_alternative_t<ID, VariantType>;
                  if(index!=ID)
                        return false;
                  result.template emplace<ID>(std::forward<ARGS>(args)...);
                  return true;
            };
            ((err!=true?(err = try_emplace.template operator()<Is>()):(err=err)),...);
            return err;
      }
};