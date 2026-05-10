#include "network/client/connection_process.h"
#include "network/client.h"
#include "network/commonsocket.h"
#include "network/abstractprocess.h"

namespace network{
    namespace connection::messaging{
        void sender<Client>::__send__(Socket sock, std::error_code& err,
                    std::shared_ptr<MessageHandler<Side::CLIENT>>& hmsg)
        {
            if(!hmsg || !hmsg->has_message()){
                err = std::make_error_code(std::errc::no_message);
                return;
            }
            err = 
                send(sock,SEND_FLAGS::NoSignal,hmsg->buffer())==-1?
                std::make_error_code(std::errc::io_error):
                std::error_code();
            return;
        }
        void sender<Client>::__bufferize__(std::error_code& err,
                std::shared_ptr<MessageHandler<Side::CLIENT>> hmsg)
        {
            if(serialization::SerializationEC err_serial =
                serialization::serialize_network(
                    static_cast<uint64_t>(serialization::serial_size(
                    *hmsg)),hmsg->buffer());
                    err_serial!=serialization::SerializationEC::NONE)
            {
                err = std::make_error_code(std::errc::bad_message);
                return;
            }
            if(serialization::SerializationEC err_serial =
                serialization::serialize_network(
                    *hmsg,hmsg->buffer());
                    err_serial!=serialization::SerializationEC::NONE)
            {
                err = std::make_error_code(std::errc::bad_message);
                return;
            }
        }
        void receiver<Client>::__receive__(Socket sock, std::error_code& err){
            auto hmsg = std::make_shared<decltype(hmsg_)::element_type>();
            using namespace serialization;
            hmsg->buffer().resize(sizeof(uint64_t));
            auto at_error = [this,hmsg](std::errc c){
                std::lock_guard lk(m_);
                hmsg_ = std::move(hmsg);
                return std::make_error_code(c);
            };
            if(receive(sock,hmsg->buffer(),hmsg->buffer().size())==-1){
                err = at_error(std::errc::io_error);
                return;
            }
            else{
                uint64_t data_sz=0;
                if(serialization::deserialize_network(data_sz,std::span<const char>(hmsg->buffer()))!=serialization::SerializationEC::NONE){
                    err = at_error(std::errc::message_size);
                    return;
                }
                hmsg->buffer().resize(data_sz+hmsg->buffer().size(),0);
                if(receive(sock,std::span<char>(hmsg->buffer()).subspan(sizeof(size_t)),hmsg->buffer().size())==-1){
                    err = at_error(std::errc::io_error);
                    return;
                }
                if(serialization::deserialize_network(*hmsg,
                        std::span<const char>(hmsg->buffer()).subspan(sizeof(size_t)))!=
                        serialization::SerializationEC::NONE)
                {
                    err = at_error(std::errc::bad_message);
                    return;
                }
                else{
                    //used for file segments transmission
                    auto has_more_msg = [this](auto&& msg){
                        if constexpr (std::is_same_v<std::decay_t<decltype(msg)>,std::monostate>)
                            this->has_more_ = false;
                        else this->has_more_.exchange(msg.message_more());
                    };
                    std::visit(has_more_msg,*hmsg);
                    return;
                }
            }
            std::lock_guard lk(m_);
            hmsg_ = std::move(hmsg);
            return;
        }
    }
}