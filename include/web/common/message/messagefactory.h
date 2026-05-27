#pragma once
#include <variant>
#include "sys/error_code.h"

template<typename VARIANT,typename = void>
    struct MessageVariantFactory: std::false_type{};

    template<typename... Types>
    struct MessageVariantFactory<std::variant<Types...>>{
        using VariantType = std::variant<Types...>;
        using ResultType = VariantType;
        static constexpr size_t var_sz(){
            return sizeof...(Types);
        }
        template<typename... ARGS>
        static ErrorCode emplace(ResultType& result, size_t index,ARGS&&... args) {
            if (index >= sizeof...(Types))
                return ErrorPrint::print_error(
                    ErrorCode::INVALID_ARGUMENT,"invalid variant type",AT_ERROR_ACTION::CONTINUE);
            return emplaceImpl(result, index, std::make_index_sequence<sizeof...(Types)>{},std::forward<ARGS>(args)...);
        }
    private:
        template <size_t... Is,typename... ARGS>
        static ErrorCode emplaceImpl(ResultType& result, size_t index, std::index_sequence<Is...>,ARGS&&... args) noexcept{
            ErrorCode err = ErrorCode::NONE;
            auto return_error = [&](const auto& emplaced) mutable noexcept{
                using Type = std::decay_t<decltype(emplaced)>;
                if constexpr(HasError<Type>){
                    err = emplaced.error();
                    if(err!=ErrorCode::NONE)
                        result.template emplace<0>();
                    return err;
                }
                else return ErrorCode::NONE;
            };
            auto try_emplace = [&]<size_t ID>(){
                using Type = std::variant_alternative_t<ID, VariantType>;
                if(index!=ID)
                    return ErrorCode::INVALID_ARGUMENT;
                if constexpr (sizeof...(ARGS)>0 && std::is_constructible_v<Type,ARGS...>)
                    return return_error(result.template emplace<ID>(std::forward<ARGS>(args)...));
                else if constexpr(sizeof...(ARGS)==0 && std::is_default_constructible_v<Type>)
                    return return_error(result.template emplace<ID>());
                else return ErrorCode::INVALID_ARGUMENT;
            };
            (((err = try_emplace.template operator()<Is>())!=ErrorCode::NONE) && ...);
            return err;
        }
    };