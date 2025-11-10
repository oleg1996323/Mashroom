#include "proc/common/fs.h"
#include "sys/log_err.h"
#include "sys/application.h"

namespace fs = std::filesystem;

ErrorCode make_dir(const fs::path& filepath) noexcept{
    if(!fs::exists(filepath.parent_path()))
        if(!fs::create_directories(filepath.parent_path())){
            log().record_log(ErrorCodeLog::CANNOT_ACCESS_PATH_X1,"",filepath.relative_path().c_str());
            return ErrorCode::INTERNAL_ERROR;
        }
    return ErrorCode::NONE;
}

ErrorCode make_file(std::ofstream& file,const std::filesystem::path& out_f_name) noexcept{
    {
        ErrorCode err = make_dir(out_f_name);
        if(err!=ErrorCode::NONE)
            return err;
    }
    file.open(out_f_name,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        if(fs::exists(out_f_name) && fs::is_regular_file(out_f_name) && fs::status(out_f_name).permissions()>fs::perms::none){
            log().record_log(ErrorCodeLog::FILE_X1_PERM_DENIED,"",out_f_name.c_str());
            return ErrorCode::INTERNAL_ERROR;
        }   
    }
    std::cout<<"Writing to "<<out_f_name<<std::endl;
    return ErrorCode::NONE;
}