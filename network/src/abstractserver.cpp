#include "abstractserver.h"

namespace network{
    template<typename DERIVED_CONNECTIONPOOL>
    void AbstractServer<DERIVED_CONNECTIONPOOL>::__accept_throw__(const Socket& socket){
        int err = errno;
        errno = 0;
        switch(err){
            #ifdef EAGAIN
            case EAGAIN:
            #elif defined EWOULDBLOCK
            case EWOULDBLOCK:
            #endif
            case EINTR:
                break;
            case ECONNABORTED:
                connection_aborted(err,socket);
                break;
            case EMFILE:
                process_max_fd_reached(err,socket);
            case ENFILE:
                system_max_fd_reached(err,socket);
                break;
            case EPERM:
                break;
            case EBADF:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
            case EPROTO:
            case EFAULT:
                throw std::runtime_error(strerror(err));
            case EINVAL:
                throw std::invalid_argument(strerror(err));
        }
    }
}