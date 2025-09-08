#pragma once
#include <vector>
#include <type_traits>
#include "network/common/def.h"
#include "network/common/message/cashed_data.h"

namespace network{
    template<Side S>
    class MessageProcess;

    class __MessageBuffer__{
        protected:
        void assign_buffer(std::vector<char>&& buf) noexcept;
        void assign_buffer(const std::vector<char>& buf) const;
        std::vector<char>& __buffer__() const;
        public:
        __MessageBuffer__() = default;
        __MessageBuffer__(const __MessageBuffer__&) = delete;
        __MessageBuffer__(__MessageBuffer__&&) noexcept = default;
        __MessageBuffer__& operator=(const __MessageBuffer__& other) = delete;
        __MessageBuffer__& operator=(__MessageBuffer__&& other) noexcept = default;
        const std::vector<char>& buffer() const{
            return __buffer__();
        }
        std::vector<char>& buffer(){
            return __buffer__();
        }
        virtual ~__MessageBuffer__();
    };
}

