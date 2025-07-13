#include "sublimed_info.h"

void SublimedDataInfo::serialize(std::ofstream& file) const{
    file.write(reinterpret_cast<const char*>(&grid_data),sizeof(decltype(grid_data)));
    size_t data_sz = buf_pos_.size();
    file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
    file.write(reinterpret_cast<const char*>(buf_pos_.data()),sizeof(decltype(buf_pos_)::value_type)*data_sz);
    file.write(reinterpret_cast<const char*>(&from),sizeof(decltype(from)));
    file.write(reinterpret_cast<const char*>(&to),sizeof(decltype(to)));
    file.write(reinterpret_cast<const char*>(&discret),sizeof(decltype(discret)));
}

void SublimedDataInfo::deserialize(std::ifstream& file){
    file.read(reinterpret_cast<char*>(&grid_data),sizeof(decltype(grid_data)));
    size_t data_sz=0;
    file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
    buf_pos_.clear();
    buf_pos_.resize(data_sz);
    file.read(reinterpret_cast<char*>(buf_pos_.data()),sizeof(decltype(buf_pos_)::value_type)*data_sz);
    file.read(reinterpret_cast<char*>(&from),sizeof(decltype(from)));
    file.read(reinterpret_cast<char*>(&to),sizeof(decltype(to)));
    file.read(reinterpret_cast<char*>(&discret),sizeof(decltype(discret)));
}

void SublimedDataInfo::serialize(std::vector<char>& buf) const{
    if(is_little_endian()){
        size_t seq_sz = htonl(sizeof(decltype(grid_data))+sizeof(size_t)+
                        buf_pos_.size()*sizeof(decltype(buf_pos_)::value_type)+sizeof(decltype(from))+sizeof(decltype(to))+sizeof(decltype(discret)));
        buf.insert(buf.end(),(const char*)&grid_data,(const char*)&grid_data+sizeof(decltype(grid_data)));
        size_t buf_pos_sz = htonl(buf_pos_.size());
        buf.insert(buf.end(),(const char*)&buf_pos_sz,(const char*)(&buf_pos_sz+sizeof(buf_pos_sz)));
        std::vector<ptrdiff_t> ptr_diff_n;
        std::transform(buf_pos_.begin(),buf_pos_.end(),std::back_inserter(ptr_diff_n),network::utility::htonll);
        buf.insert(buf.end(),(const char*)ptr_diff_n.data(),(const char*)(ptr_diff_n.data()+sizeof(decltype(ptr_diff_n)::value_type)*ptr_diff_n.size()));
        uint64_t from_n = network::utility::htonll(from.time_since_epoch().count());
        buf.insert(buf.end(),(const char*)&from_n,(const char*)&from_n+sizeof(decltype(from_n)));
        uint64_t to_n = network::utility::htonll(to.time_since_epoch().count());
        buf.insert(buf.end(),(const char*)&to_n,(const char*)&to_n+sizeof(decltype(to_n)));
        uint64_t discret_n = network::utility::htonll(discret.count());
        buf.insert(buf.end(),(const char*)&discret_n,(const char*)&discret_n+sizeof(decltype(discret_n)));
    }
    else{
        size_t seq_sz = sizeof(decltype(grid_data))+sizeof(size_t)+
                        buf_pos_.size()*sizeof(decltype(buf_pos_)::value_type)+sizeof(decltype(from))+sizeof(decltype(to))+sizeof(decltype(discret));
        buf.insert(buf.end(),(const char*)&seq_sz,(const char*)&seq_sz+sizeof(decltype(seq_sz)));
        buf.insert(buf.end(),(const char*)&grid_data,(const char*)&grid_data+sizeof(decltype(grid_data)));
        size_t buf_pos_sz = buf_pos_.size();
        buf.insert(buf.end(),(const char*)&buf_pos_sz,(const char*)(&buf_pos_sz+sizeof(buf_pos_sz)));
        buf.insert(buf.end(),(const char*)buf_pos_.data(),(const char*)(buf_pos_.data()+sizeof(decltype(buf_pos_)::value_type)*buf_pos_.size()));
        uint64_t from_n = from.time_since_epoch().count();
        buf.insert(buf.end(),(const char*)&from_n,(const char*)&from_n+sizeof(decltype(from_n)));
        uint64_t to_n = to.time_since_epoch().count();
        buf.insert(buf.end(),(const char*)&to_n,(const char*)&to_n+sizeof(decltype(to_n)));
        uint64_t discret_n = discret.count();
        buf.insert(buf.end(),(const char*)&discret_n,(const char*)&discret_n+sizeof(decltype(discret_n)));
    }
}

void SublimedDataInfo::deserialize(const std::vector<char>& buf,size_t n){
    size_t read = n;
    assert(buf.size()>read);
    std::memcpy(&grid_data,buf.data()+read,sizeof(decltype(grid_data)));
    read+=sizeof(decltype(grid_data));
    assert(buf.size()>read);
    std::memcpy(&from,buf.data()+read,sizeof(decltype(from)));
    read+=sizeof(decltype(from));
    assert(buf.size()>read);
    std::memcpy(&to,buf.data()+read,sizeof(decltype(to)));
    read+=sizeof(decltype(to));
    assert(buf.size()>read);
    std::memcpy(&discret,buf.data()+read,sizeof(decltype(discret)));
}