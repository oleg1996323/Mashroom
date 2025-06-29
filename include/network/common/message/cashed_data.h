#pragma once
#include <unordered_map>
#include <vector>
#include <cassert>

namespace network{
    struct __MessageBuffer__;
}

class CashedData{
    static std::unordered_map<const network::__MessageBuffer__*,std::vector<char>> data_;
    public:
    template<typename T>
    static std::vector<char>& add(const T* msg_ptr){
        assert(msg_ptr);
        return data_[msg_ptr];
    }
    template<typename T>
    static void erase(const T* msg_ptr){
        assert(msg_ptr);
        data_.erase(msg_ptr);
    }
    template<typename T>
    static std::vector<char>& get(const T* msg_ptr){
        assert(msg_ptr);
        return data_[msg_ptr];
    }
};

inline std::unordered_map<const network::__MessageBuffer__*,std::vector<char>> CashedData::data_ = {};