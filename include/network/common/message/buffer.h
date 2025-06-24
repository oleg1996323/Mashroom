#pragma once
#include <vector>
#include "network/common/def.h"

namespace network{
    template<Side S>
    class MessageProcess;

    class __MessageBuffer__{
        protected:
        void assign_buffer(std::vector<char>&& buf) noexcept;
        void assign_buffer(const std::vector<char>& buf) const;
        std::vector<char>& __buffer__() const;
        public:
        const std::vector<char>& buffer() const{
            return __buffer__();
        }
        std::vector<char>& buffer(){
            return __buffer__();
        }
    };
}