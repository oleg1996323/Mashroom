#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
template<typename T>
consteval T& PseudoRefType() noexcept{
    char byte_seq[sizeof(T)];
    return reinterpret_cast<T&>(*byte_seq);
}
#pragma GCC diagnostic pop