#pragma once
#include <program/data.h>

namespace network::client::detail{
    namespace capitalize{
        template<Data::TYPE,Data::FORMAT>
        struct __Message__;

        template<Data::TYPE T,Data::FORMAT F>
        struct __MessageProxy__{
            static constexpr Data::TYPE data_type = T;
            static constexpr Data::FORMAT data_format = F;
            static std::vector<char> serialize(const __Message__<T,F>& msg, const Data& data){
                return __Message__<T,F>::serialize(msg);
            }
            static __Message__<T,F> deserialize(std::vector<char>::const_iterator buffer_iter){
                return __Message__<T,F>::deserialize(buffer_iter);
            }
        };

        template<>
        struct __Message__<Data::TYPE::METEO,Data::FORMAT::GRIB>:
        __MessageProxy__<Data::TYPE::METEO,Data::FORMAT::GRIB>
        {
            size_t buffer_size_;
            std::vector<char> buffer_;

            static std::vector<char> serialize(const __Message__<Data::TYPE::METEO,Data::FORMAT::GRIB>& msg, const Data& data){
                std::vector<char> buffer;
                for(auto [nn_file,cmn_sublimed_d]:data.data()){
                    
                }
                return buffer;
            }
            static __Message__<Data::TYPE::METEO,Data::FORMAT::GRIB> deserialize(std::vector<char>::const_iterator buffer_iter){
                __Message__<Data::TYPE::METEO,Data::FORMAT::GRIB> result;
                
                return result;
            } 
            //TODO: complete
        };
    }
}