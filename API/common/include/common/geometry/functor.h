#pragma once

namespace geometry{
    template<template<typename...> typename DERIVED,typename... ARGS>
    struct Functor{
        using type = DERIVED<ARGS...>;
        using caller = (&DERIVED<ARGS...>::operator());
        static constexpr uint64_t args_sz() noexcept{
            return sizeof...(ARGS);
        }
    };
}