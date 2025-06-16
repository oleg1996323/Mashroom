#pragma once
#include <network/common/def.h>
#include <network/common/message/msgdef.h>

namespace network{
    template<Side S,TYPE_MESSAGE<S>::type MSG>
    struct HeaderBase;

    template<Side S,TYPE_MESSAGE<S>::type MSG>
    struct HeaderAdditionPrivate;

    template<Side S,TYPE_MESSAGE<S> MSG>
    class MessageBase;

    template<TYPE_MESSAGE<Side::SERVER>::type MSG>
    struct HeaderBase<Side::SERVER,MSG>{
        uint64_t data_sz_ = 0;
        static constexpr TYPE_MESSAGE<S> type_msg_ = MSG;
        network::server::Status status_=network::server::Status::READY;

        constexpr static size_t serial_size(){
            return sizeof(data_sz_)+sizeof(type_msg_)+sizeof(status_);
        }
        constexpr static size_t begin_byte(){
            return 0;
        }

        HeaderBase(server::Status status,size_t data_sz);
        HeaderBase(HeaderBase&& other) noexcept{
            *this=std::move(other);
        }
        HeaderBase& operator=(HeaderBase&& other){
            if(this!=&other){
                data_sz_= other.data_sz_;
                status_ = other.status_;
            }
            return *this;
        }
    };

    template<TYPE_MESSAGE<Side::CLIENT>::type MSG>
    struct HeaderBase<Side::CLIENT,MSG>{
        uint64_t data_sz_ = 0;
        static constexpr TYPE_MESSAGE<S> type_msg_ = MSG;

        constexpr static size_t serial_size(){
            return sizeof(data_sz_)+sizeof(type_msg_);
        }
        constexpr static size_t begin_byte(){
            return 0;
        }

        HeaderBase(server::Status status,size_t data_sz);
        HeaderBase(HeaderBase&& other) noexcept{
            *this=std::move(other);
        }
        HeaderBase& operator=(HeaderBase&& other){
            if(this!=&other)
                data_sz_= other.data_sz_;
            return *this;
        }
    };

    template<>
    struct HeaderAdditionPrivate<Side::CLIENT,TYPE_MESSAGE<Side::CLIENT>::SERVER_STATUS>{
        constexpr static size_t serial_size(){
            return 0;
        }
    };


    /// @todo add different kind of type data and items to request
    template<>
    struct HeaderAdditionPrivate<Side::CLIENT,TYPE_MESSAGE<Side::CLIENT>::DATA_REQUEST>{
        std::optional<TimeFrame> t_fcst_;
        std::optional<uint16_t> parameter_sz_;
        std::optional<Organization> center;
        std::optional<utc_tp> from;   //requested from Date
        std::optional<utc_tp> to;     //requested to Date
        std::optional<Coord> pos;   //requested position (in WGS)
        std::optional<RepresentationType> rep_t;
        std::optional<TimeOffset> time_off_;
        std::optional<Extract::ExtractFormat> fmt_ = Extract::ExtractFormat::TXT_F|Extract::ExtractFormat::ARCHIVED;
        constexpr static size_t serial_size(){
            return 0;
        }
    };

    template<Side S,TYPE_MESSAGE<S>::type MSG>
    struct HeaderAddition:HeaderAditionPrivate<S,MSG>{
        constexpr static size_t serial_size(){
            return HeaderAditionPrivate<S,MSG>::serial_size();
        }
        constexpr static size_t begin_byte(){
            return HeaderBase<S,MSG>::begin_byte();
        }
    };

    template<Side S,TYPE_MESSAGE<S> MSG>
    class MessageBase{
        HeaderBase<S,MSG> base_;
        protected:
        void assign_buffer(std::vector<char>&& buf) noexcept;
        void assign_buffer(const std::vector<char>& buf) const;
        std::vector<char>& __buffer__() const;
        public:
        const std::vector<char>& buffer() const{
            return __buffer__();
        }
        HeaderBase& operator=(HeaderBase&& other){
            if(this!=&other){
                data_sz_= other.data_sz_;
                __buffer__().swap(other.__buffer__());
            }
            return *this;
        }
    };
}