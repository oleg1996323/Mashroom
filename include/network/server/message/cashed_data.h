#pragma once
#include <unordered_map>
#include <vector>
#include <cassert>

class CashedData{
    static std::unordered_map<void*,std::vector<char>> data_;
    public:
    static std::vector<char>& add(void* msg_ptr){
        assert(msg_ptr);
        return data_[msg_ptr];
    }
    static void erase(void* msg_ptr){
        assert(msg_ptr);
        data_.erase(msg_ptr);
    }
    static std::vector<char>& get(void* msg_ptr){
        assert(msg_ptr);
        return data_[msg_ptr];
    }
};
