#include "commonclient.h"

namespace network{
template<typename PROCESS_T>
void CommonClient<PROCESS_T>::__connect_throw__(){
    int err = errno;
    errno = 0;
    switch(err){
        case EACCES:
        case EPERM:
        case EADDRINUSE:
        case EAGAIN:
        case EALREADY:
        case EBADF:
        case ECONNREFUSED:
        case EFAULT:
        case EINPROGRESS:
        case EISCONN:
        case ENETUNREACH:
        case ENOTSOCK:
        case ETIMEDOUT:
            throw std::runtime_error(strerror(err));
            break;
        case EADDRNOTAVAIL:
        case EAFNOSUPPORT:
        case EPROTOTYPE:
            throw std::invalid_argument(strerror(err));
            break;
        case EINTR:
            return;
            break;
        default:
            break;
        return;
    }
}
}