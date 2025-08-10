#pragma once

#define ENABLE_DERIVED_VARIANT(Derived, BaseVariant)\
    template<>\
    struct std::variant_size<Derived>\
          : std::variant_size<BaseVariant> {};\
    template<std::size_t I>\
    struct std::variant_alternative<I, Derived>\
          : std::variant_alternative<I, BaseVariant> {} 
