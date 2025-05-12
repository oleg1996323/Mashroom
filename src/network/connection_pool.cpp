#include <network/connection_pool.h>

using namespace connection;

ConnectionPool::iterator ConnectionPool::add_connection(int connection_socket,size_t buffer_sz){
    std::lock_guard<std::mutex> lock(locker_);
    return data_.insert(Process(connection_socket,*this,buffer_sz)).first;
}
void connection::ConnectionPool::update_connections(){
    std::erase_if(data_,[this](const std::decay_t<decltype(data_)>::value_type& process){
        return !is_connected(process.socket());
    });
}
#include <network/server.h>
void connection::ConnectionPool::__proc_notifier__(connection::ConnectionPool& connections_pool){
    while(connections_pool.is_running_.load()){
        std::unique_lock<std::mutex> l(connections_pool.locker_);
        connections_pool.notiifier_cv_.wait(l,[&connections_pool](){return !connections_pool.data_.empty();});
        l.lock();
        if(!connections_pool.data_.empty()){
            connections_pool.update_connections();
            for(auto& conn:connections_pool.data_){
                ErrorCode err = conn.send_status_message(connections_pool.server_.get_status());
                if(err!=ErrorCode::NONE){
                    ErrorPrint::print_error(err,"status notify",AT_ERROR_ACTION::CONTINUE);
                    close(conn.socket());
                }
            }
        }
        l.unlock();
    }
}
connection::ConnectionPool::ConnectionPool(const Server& server):server_(server){
    thread_ = std::jthread(__proc_notifier__,std::ref(*this));
}
connection::ConnectionPool::~ConnectionPool(){
    std::lock_guard<std::mutex> lock(locker_);
    data_.clear();
    thread_.join();
}
void connection::ConnectionPool::shutdown_all(){
    std::for_each(data_.begin(),data_.end(),[](const connection::Process& conn){
        return conn.shut();
    });
}
void connection::ConnectionPool::shut_not_processing(){
    for(auto& conn:data_)
        if(!conn.busy())
            conn.shut();
        else conn.shut_at_done();
}
void connection::ConnectionPool::close_all_at_not_busy(){
    std::for_each(data_.begin(),data_.end(),[](const connection::Process& conn){
        if(!conn.busy())
            return conn.close();
        else return conn.close_at_done();
    });
}
void connection::ConnectionPool::close_all(){
    std::for_each(data_.begin(),data_.end(),[](const connection::Process& conn){
        return conn.close();
    });
}
server::Status ConnectionPool::server_status() const{
    return server_.get_status();
}
ErrorCode ConnectionPool::process_connection(Socket sock) const{
    if(data_.contains(sock)){
        auto found = data_.find(sock);
        return found->execute();
    }
    else return ErrorCode::CONNECTION_CLOSED;
}
void ConnectionPool::notify_status_all() const{
    for(auto& conn:data_)
        if(!conn.busy())
            conn.send_status_message(server_status());
}
bool ConnectionPool::is_connected(Socket socket) const{
    if(data_.contains(socket))
        return data_.find(socket)->is_connected();
    else return false;
}