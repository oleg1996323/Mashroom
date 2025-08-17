#pragma once
#include <variant>
#include "concepts.h"

#define ENABLE_DERIVED_VARIANT(Derived, BaseVariant)\
    template<>\
    struct std::variant_size<Derived>\
          : std::variant_size<BaseVariant> {};\
    template<std::size_t I>\
    struct std::variant_alternative<I, Derived>\
          : std::variant_alternative<I, BaseVariant> {} 

template<typename Variant>
requires (IsStdVariant<Variant>)
struct VariantFactory{
      using VariantType = Variant;
      using ResultType = VariantType;

      template<typename... ARGS>
      static bool emplace(ResultType& result, size_t index,ARGS&&... args) {
            if (index >= std::variant_size_v<Variant>)
                  return false;
            return emplaceImpl(result, index, std::make_index_sequence<std::variant_size_v<Variant>>{},std::forward<ARGS>(args)...);
      }
private:
      template <size_t... Is,typename... ARGS>
      static bool emplaceImpl(ResultType& result, size_t index, std::index_sequence<Is...>,ARGS&&... args) noexcept{
            bool err = false;
            auto try_emplace = [&]<size_t ID>()
            {
                  static_assert(((sizeof...(ARGS)>0?std::is_constructible_v<std::variant_alternative_t<ID, VariantType>,ARGS...>:false) || 
                              (sizeof...(ARGS)==0?std::is_default_constructible_v<std::variant_alternative_t<ID, VariantType>>:false)));
                  using Type = typename std::variant_alternative<ID, VariantType>::type;
                  if(index!=ID)
                        return false;
                  if constexpr (sizeof...(ARGS)>0)
                        result.template emplace<typename std::variant_alternative<ID,VariantType>::type>(std::forward<ARGS>(args)...);
                  else result.template emplace<typename std::variant_alternative<ID,VariantType>::type>();
                  return true;
            };
            ((err!=true?(err = try_emplace.template operator()<Is>()):(err=err)),...);
            return err;
      }
};