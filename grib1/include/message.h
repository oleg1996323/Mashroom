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
#include <filesystem>
#include <string>
#include <string_view>
namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;

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
                                    section_2_(section_1_.section1Flags().sec2_inc?section_1_.buffer_+section_1_.section_length():nullptr),
                                    section_3_(section_1_.section1Flags().sec3_inc?nullptr:(section_1_.section1Flags().sec2_inc?
                                            section_2_.buf_+section_2_.section_length():section_1_.buffer_+section_1_.section_length())),
                                    section_4_(section3().has_value()?section_3_.buf_:
                                                section2().has_value()?section_2_.buf_:
                                                section_1_.buffer_
                                                    +(section3().has_value()?section_3_.section_length():(section2().has_value()?
                                                            section_2_.section_length():section_1_.section_length()))){}

    unsigned long message_length() const noexcept{
        return section_0_.message_length();
    }
    const IndicatorSection& section0() const {
        return section_0_;
    }
    const ProductDefinitionSection& section1() const{
        return section_1_;
    }
    std::optional<std::reference_wrapper<const GridDescriptionSection>> section2() const{
        if(section_1_.section1Flags().sec2_inc)
            return std::cref(section_2_);
        else return std::nullopt;
    }
    std::optional<std::reference_wrapper<const BitMapSection>> section3() const{
        if(section_1_.section1Flags().sec3_inc)
            return std::cref(section_3_);
        else return std::nullopt;
    }
    const BinaryDataSection& section4() const{
        return section_4_;
    }
    float extract_value(int n);
    std::vector<float> extract_all();
    #endif
}
STRUCT_END(Message)

STRUCT_BEG(HGrib1)
{   
    #ifdef __cplusplus
    private:
    #endif
    std::unique_ptr<Message> msg_ DEF_STRUCT_VAL(nullptr)
    unsigned char* __f_ptr DEF_STRUCT_VAL(nullptr)
    unsigned char* current_ptr_ DEF_STRUCT_VAL(nullptr)
    unsigned long sz_ DEF_STRUCT_VAL(0)
    FILE* file = nullptr;
    #ifdef __cplusplus
    public:
    HGrib1(const fs::path& filename):msg_(nullptr){open_grib(filename);}
    HGrib1() = default;
    ~HGrib1(){
        if(__f_ptr)
            munmap(__f_ptr,lseek(fileno(file),0,SEEK_END));
    }
    std::optional<std::reference_wrapper<Message>> message() const{
        if(msg_)
            return *msg_;
        else return std::nullopt;
    }
    ptrdiff_t current_message_position() const noexcept{
        return static_cast<ptrdiff_t>(current_ptr_-__f_ptr);
    }
    std::optional<unsigned long> current_message_length() const noexcept{
        if(msg_)
            return msg_->section_0_.message_length();
        else return std::nullopt;
    }
    bool next_message(){
        if(msg_){
            if((current_ptr_-__f_ptr)+msg_->section_0_.message_length()<sz_){
                current_ptr_+=msg_->section_0_.message_length();
                return true;
            }
            return false;
        }
        return false;
    }
    std::optional<unsigned long> file_size() const noexcept{
        if(msg_){
            return sz_;
        }
        return std::nullopt;
    }
    bool is_correct_format() const noexcept{
        if(msg_ && sz_>=sec_0_min_sz)
            if(memcmp(msg_->section_0_.buf_,"GRIB",4))
                return true;
        return false;
    }
    std::optional<unsigned char> grib_version() const noexcept{
        if(is_correct_format())
            return msg_->section_0_.grib_version();
    }
    ErrorCodeData open_grib(const fs::path& filename){
        msg_ = nullptr;
        __f_ptr = nullptr;
        current_ptr_ = nullptr;
        sz_ = 0;
        if(file)
            fclose(file);
        file = fopen(filename.string().c_str(),"rb");
        if(!file)
            return ErrorCodeData::OPEN_ERROR;
        fseek(file, 0, SEEK_END);
        sz_ = ftell(file);
        fseek(file,0,SEEK_SET);
        __f_ptr = (unsigned char*)mmap(NULL,sz_,PROT_READ,MAP_FILE|MAP_SHARED,fileno(file),0);
        if(!__f_ptr)
            return ErrorCodeData::READ_POS;
        current_ptr_ = __f_ptr;
        msg_ = std::make_unique<Message>(__f_ptr);
        try{
            const auto tmp = current_message_length();
            if(tmp.has_value()){
                if(!memcmp(__f_ptr+current_message_length().value()-4,"7777",4))
                    return ErrorCodeData::MISS_END_SECTION;
            }
            else return ErrorCodeData::DATA_EMPTY;
        }
        catch(...){
            return ErrorCodeData::BAD_FILE;
        }
        return ErrorCodeData::NONE_ERR;
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