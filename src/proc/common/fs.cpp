#include "proc/common/fs.h"
#include "sys/application.h"
#include "sys/error.h"

namespace fs = std::filesystem;

void make_and_open_file(std::ofstream& file,const std::filesystem::path& out_f_name){
    if(!fs::exists(out_f_name.parent_path()) && !fs::create_directories(out_f_name.parent_path())){
        osterlib::ContextedError ctx_err(mashroom::errc::no_exists_path);
        ctx_err.with_field("path",out_f_name.relative_path().c_str());
        throw ctx_err;
    }
    file.open(out_f_name,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        if(fs::exists(out_f_name)){
            osterlib::ContextedError ctx_err(mashroom::errc::no_exists_path);
            ctx_err.with_field("path",out_f_name.c_str());
            throw ctx_err;
        }
        else if(fs::is_regular_file(out_f_name)){
            osterlib::ContextedError ctx_err(mashroom::errc::not_file);
            ctx_err.with_field("path",out_f_name.c_str());
            throw ctx_err;
        }
        else if(fs::status(out_f_name).permissions()>fs::perms::none){
            osterlib::ContextedError ctx_err(mashroom::errc::file_permission_denied);
            ctx_err.with_field("path",out_f_name.c_str());
            throw ctx_err;
        }
    }
}