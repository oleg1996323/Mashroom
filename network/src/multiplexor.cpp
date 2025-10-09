#include "multiplexor.h"

void network::Multiplexor::__epoll_ctl_throw__(){
    int err = errno;
    errno = 0;
    switch(err){
        case EPERM:
        case ENOMEM:
        case EBADF:
            throw std::runtime_error(strerror(err));
            break;
        case EEXIST:
        case EINVAL:
        case ENOENT:
            throw std::invalid_argument(strerror(err));
            break;
        case ELOOP:
        case ENOSPC:
            throw std::logic_error(strerror(err));
            break;
        default:
            throw std::runtime_error(strerror(err));
        break;
    }
}

void network::Multiplexor::__epoll_wait_throw__(){
    int err = errno;
    errno = 0;
    switch(err){
        case EBADF:
        case EFAULT:
        throw std::runtime_error(strerror(err));
            break;
        case EINTR:
        //ignore interruptions
            return;
            break;
        case EINVAL:
            throw std::invalid_argument(strerror(err));
        default:
            throw std::runtime_error(strerror(err));
    }
}

void network::Multiplexor::__set_interruptor__(){
    if(!interruptor){
        if(int ev = eventfd(0,EFD_NONBLOCK);ev==-1)
            throw std::runtime_error(strerror(errno));
        else interruptor = std::move(std::unique_ptr<Interruptor>(new Interruptor(ev)));
        epoll_event ev{.events = Event::In|Event::EdgeTrigger};
        if(epoll_ctl(epollfd,EPOLL_CTL_ADD,interruptor->fd_,&ev)==-1)
            __epoll_ctl_throw__();
        }
}
network::Multiplexor::Multiplexor(size_t mp_controled):
events_([mp_controled](){
    std::vector<epoll_event> res;
    res.resize(mp_controled);
    return res;
}()),
epollfd(epoll_create(mp_controled)){
    __set_interruptor__();
}
network::Multiplexor::~Multiplexor(){
    close(epollfd);
}

network::Multiplexor& network::Multiplexor::add(const Socket& socket,Event event){
    epoll_event ev{.events = event,.data = epoll_data_t{.fd = socket.__descriptor__()}};
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,socket.__descriptor__(),&ev)==-1)
        __epoll_ctl_throw__();
    return *this;
}
network::Multiplexor& network::Multiplexor::remove(const Socket& socket){
    if(epoll_ctl(epollfd,EPOLL_CTL_DEL,socket.__descriptor__(),nullptr)==-1)
        __epoll_ctl_throw__();
    return *this;
}
network::Multiplexor& network::Multiplexor::modify(const Socket& socket,Event event){
    epoll_event ev{.events = event,.data = epoll_data_t{.fd = socket.__descriptor__()}};
    if(epoll_ctl(epollfd,EPOLL_CTL_MOD,socket.__descriptor__(),&ev)==-1)
        __epoll_ctl_throw__();
    return *this;
}
void network::Multiplexor::interrupt() noexcept{
    assert(interruptor);
    interruptor->interrupted = true;
    write(interruptor->fd_, &interruptor->interrupted, sizeof(interruptor->interrupted));
    std::cout<<"Interrupted multiplexor"<<std::endl;
    //read(interruptor->fd_,&interruptor->interrupted,sizeof(interruptor->interrupted));
}
bool network::Multiplexor::interrupted() noexcept{
    if(interruptor)
        return interruptor->interrupted;
    else return false;
}
/**
 * @param timeout - in milliseconds
 * If timeout == -1 than infinite timeout is set.
 */
std::span<network::Multiplexor::Event_t> network::Multiplexor::wait(int32_t timeout){
    if(int event_sz = epoll_wait(epollfd,events_.data(),events_.size(),timeout);event_sz==-1){
        //std::cout<<"Multiplexor error occured"<<std::endl;
        __epoll_wait_throw__();
        return std::span<network::Multiplexor::Event_t>();
    }
    else{
        //std::cout<<"Multiplexor found events"<<std::endl;
        return std::span(events_).subspan(0,event_sz);
    }
}

network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)|
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)&
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator^(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)^
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator~(network::Multiplexor::Event val) noexcept{
    return static_cast<network::Multiplexor::Event>(~static_cast<std::underlying_type_t<network::Multiplexor::Event>>(val));
}
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,int rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)&
            rhs);
}
network::Multiplexor::Event operator&(std::underlying_type_t<network::Multiplexor::Event> lhs, network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(lhs &
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,std::underlying_type_t<network::Multiplexor::Event> rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)|
            rhs);
}
network::Multiplexor::Event operator|(std::underlying_type_t<network::Multiplexor::Event> lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(lhs |
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}