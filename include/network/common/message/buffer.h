#pragma once
#include <vector>

namespace network{
    template<Side S>
    class MessageProcess;

    class __MessageBuffer__{
        protected:
        void assign_buffer(std::vector<char>&& buf) noexcept;
        void assign_buffer(const std::vector<char>& buf) const;
        std::vector<char>& __buffer__() const;
        template<Side S>
        friend class MessageProcess;
        public:
        const std::vector<char>& buffer() const{
            return __buffer__();
        }
    };
}