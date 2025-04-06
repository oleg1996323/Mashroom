#pragma once
#include "aux_code/def.h"
#include "def.h"
#include "sections/section_0.h"
#include "sections/section_1.h"
#include "sections/section_2.h"
#include "sections/section_3.h"
#include "sections/section_4.h"
#include "sys/mman.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#ifdef __cplusplus
#include <filesystem>
#include <string>
#include <string_view>
namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;
#endif

STRUCT_BEG(Message)
{
    IndicatorSection section_0_;
    ProductDefinitionSection section_1_;
    GridDescriptionSection section_2_;
    BitMapSection section_3_;
    BinaryDataSection section_4_;
    ErrorCodeData err_;

    #ifdef __cplusplus
    Message(unsigned char* buffer):section_0_(buffer),
                                    section_1_(buffer+sec_0_min_sz),
                                    section_2_(buffer+sec_0_min_sz+section_1_.section_length()),
                                    section_3_(section_2_.buf_+section_2_.section_length()),
                                    section_4_(section_3_.buf_+section_3_.section_length()){}

    unsigned long message_length() const noexcept{
        return section_0_.message_length();
    }
    float extract_value(int n);
    #endif
}
STRUCT_END(Message)

STRUCT_BEG(HGrib1)
{   
    #ifdef __cplusplus
    private:
    #endif
    Message msg_ DEF_STRUCT_VAL(nullptr)
    unsigned char* __f_ptr DEF_STRUCT_VAL(nullptr)
    unsigned char* current_ptr_ DEF_STRUCT_VAL(nullptr)
    unsigned long sz_ DEF_STRUCT_VAL(0)
    FILE* file DEF_STRUCT_VAL(nullptr);
    #ifdef __cplusplus
    public:
    HGrib1(const fs::path& filename):msg_(nullptr){open_grib(filename);}
    HGrib1() = default;
    ~HGrib1(){
        if(__f_ptr)
            munmap(__f_ptr,lseek(fileno(file),0,SEEK_END));
    }
    Message& message(){
        return msg_;
    }
    void next_message(){
        
    }
    unsigned long file_size() const noexcept{
        return sz_;
    }
    bool is_correct_format() const noexcept{
        if(sz_>=sec_0_min_sz)
            if(msg_.section_0_.buf_[0]=='G' && msg_.section_0_.buf_[1]=='R' && msg_.section_0_.buf_[2]=='I' && msg_.section_0_.buf_[3]=='B')
                return true;
        return false;
    }
    unsigned char grib_version() const noexcept{
        if(is_correct_format())
            return msg_.section_0_.grib_version();
    }
    bool open_grib(const fs::path& filename){
        msg_ = nullptr;
        __f_ptr = nullptr;
        current_ptr_ = nullptr;
        sz_ = 0;
        if(file)
            fclose(file);
        file = fopen(filename.c_str(),O_RDONLY);
        if(!file)
            throw std::runtime_error("Cannot open file");
        fseek(file, 0, SEEK_END);
        sz_ = ftell(file);
        fseek(file,0,SEEK_SET);
        __f_ptr = (unsigned char*)mmap(NULL,sz_,PROT_READ,MAP_FILE|MAP_SHARED,fileno(file),0);
        if(!__f_ptr)
            throw std::runtime_error("Reading error");
        current_ptr_ = __f_ptr;
        msg_ = Message(__f_ptr);
    }
    #endif
}
STRUCT_END(HGrib1)

#ifndef __cplusplus
Message* read_message(Message* msg){

};
Message* next_message(Message* msg){

}
#endif