#include "proc/common/fs.h"
#include "sys/log_err.h"
#include "sys/application.h"
#include "sys/error_exception.h"

namespace fs = std::filesystem;

void make_dir(const fs::path& filepath){
    if(!fs::exists(filepath.parent_path()))
        if(!fs::create_directories(filepath.parent_path()))
            throw ErrorException(ErrorCode::CANNOT_ACCESS_PATH_X1,std::string_view(),filepath.relative_path().c_str());
}

void make_file(std::ofstream& file,const std::filesystem::path& out_f_name){
    make_dir(out_f_name);
    file.open(out_f_name,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        if(fs::exists(out_f_name) && fs::is_regular_file(out_f_name) && fs::status(out_f_name).permissions()>fs::perms::none)
            throw ErrorException(ErrorCode::FILE_X1_PERM_DENIED,std::string_view(),out_f_name.c_str());
    }
    std::cout<<"Writing to "<<out_f_name<<std::endl;
}