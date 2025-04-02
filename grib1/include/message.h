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
    #endif
}
STRUCT_END(Message)


STRUCT_BEG(HGrib1)
{
    Message msg_ DEF_STRUCT_VAL(nullptr);
    unsigned char* __f_ptr DEF_STRUCT_VAL(nullptr);
    unsigned char* current_ptr_ DEF_STRUCT_VAL(nullptr)
    int fno DEF_STRUCT_VAL(-1);
    #ifdef __cplusplus
    HGrib1(const fs::path& filename) try: msg_(nullptr),__f_ptr(nullptr),current_ptr_(nullptr),fno(open(filename.c_str(),O_RDONLY))
    {
        if(fno==-1)
            throw std::runtime_error("Unable to open file "s+filename.c_str());
        __f_ptr = (unsigned char*)mmap(NULL,lseek(fno,0,SEEK_END),PROT_READ,MAP_FILE|MAP_SHARED,fno,0);
        current_ptr_ = __f_ptr;
        msg_ = Message(__f_ptr);
    }
    catch(const std::runtime_error& err){

    }

    ~HGrib1(){
        if(__f_ptr)
            munmap(__f_ptr,lseek(fno,0,SEEK_END));
    }

    void next_message(){
        
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