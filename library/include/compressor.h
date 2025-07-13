#pragma once
#include <zip.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace cpp::zip_ns{
    class Compressor{
        public:
        Compressor(const fs::path& zip_path):path_(zip_path){
            zip_ = zip_open(zip_path.c_str(),ZIP_CREATE|ZIP_TRUNCATE,&zip_err_code_);
            if(zip_err_code_){
                zip_error error_at_open;
                zip_error_init_with_code(&error_at_open,zip_err_code_);
                throw std::runtime_error(zip_error_strerror(&error_at_open));
                return;
            }
        }

        Compressor(const fs::path& zip_dir, const std::string& zip_name):path_(zip_dir/zip_name){
            zip_ = zip_open(path_.c_str(),ZIP_CREATE|ZIP_TRUNCATE,&zip_err_code_);
            if(zip_err_code_){
                zip_error error_at_open;
                zip_error_init_with_code(&error_at_open,zip_err_code_);
                throw std::runtime_error(zip_error_strerror(&error_at_open));
                return;
            }
        }

        void add_file(const fs::path& rel_dir, const fs::path& file_path){
            if(zip_!=NULL){
                zip_source* zs = zip_source_file(zip_,file_path.string().c_str(),0,0);
                if(!zs){
                    std::cout<<"Unable to create source of file. Abort."<<std::endl;
                    exit(1);
                }
                if(zip_file_add(zip_,fs::relative(file_path,rel_dir).string().c_str(), zs,ZIP_FL_ENC_UTF_8)<0){
                    std::cerr << "Failed to add file to archive: " << file_path << std::endl;
                    zip_source_free(zs);
                    exit(1);
                }
            }
        }

        const fs::path& get_zip_path() const{
            return path_;
        }

        ~Compressor(){
            if(zip_)
                zip_close(zip_);
        }

        private:
        const fs::path& path_;
        zip* zip_ = nullptr;
        int zip_err_code_ = 0;
    };
}