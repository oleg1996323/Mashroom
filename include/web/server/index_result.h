#pragma once
#include "program/data.h"

namespace network::client::detail{
    namespace index{
        template<Data_t,Data_f>
        struct __Message__;

        template<Data_t T,Data_f F>
        struct __MessageProxy__{
            static constexpr Data_t data_type = T;
            static constexpr Data_f data_format = F;
            static std::vector<char> serialize(const __Message__<T,F>& msg, const Data& data){
                return __Message__<T,F>::serialize(msg);
            }
            static __Message__<T,F> deserialize(std::vector<char>::const_iterator buffer_iter){
                return __Message__<T,F>::deserialize(buffer_iter);
            }
        };

        template<>
        struct __Message__<Data_t::TIME_SERIES,Data_f::GRIB_v1_v1>:
        __MessageProxy__<Data_t::TIME_SERIES,Data_f::GRIB_v1_v1>
        {
            size_t buffer_size_;
            std::vector<char> buffer_;

            static std::vector<char> serialize(const __Message__<Data_t::TIME_SERIES,Data_f::GRIB_v1_v1>& msg, const Data& data){
                std::vector<char> buffer;
                for(auto [nn_file,cmn_sublimed_d]:data.data()){
                    
                }
                return buffer;
            }
            static __Message__<Data_t::TIME_SERIES,Data_f::GRIB_v1_v1> deserialize(std::vector<char>::const_iterator buffer_iter){
                __Message__<Data_t::TIME_SERIES,Data_f::GRIB_v1_v1> result;
                
                return result;
            } 
            //TODO: complete
        };
    }
}