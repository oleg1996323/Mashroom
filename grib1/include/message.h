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
#include <iostream>
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
                                    section_4_(section3().has_value()?section_3_.buf_+section_3_.section_length():
                                                section2().has_value()?section_2_.buf_+section_2_.section_length():
                                                section_1_.buffer_+section_1_.section_length()){}

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
    std::unique_ptr<::Message> msg_ DEF_STRUCT_VAL(nullptr)
    unsigned char* __f_ptr DEF_STRUCT_VAL(nullptr)
    unsigned char* current_ptr_ DEF_STRUCT_VAL(nullptr)
    unsigned long sz_ DEF_STRUCT_VAL(0)
    int file = -1;
    #ifdef __cplusplus
    public:
    HGrib1(const fs::path& filename):msg_(nullptr){open_grib(filename);}
    HGrib1() = default;
    ~HGrib1();
    std::optional<std::reference_wrapper<::Message>> message() const;
    ptrdiff_t current_message_position() const noexcept;
    std::optional<unsigned long> current_message_length() const noexcept;
    bool next_message();
    std::optional<unsigned long> file_size() const noexcept;
    bool is_correct_format() const noexcept;
    std::optional<unsigned char> grib_version() const noexcept;
    ErrorCodeData open_grib(const fs::path& filename);
    bool set_message(ptrdiff_t pos) noexcept;
    #endif
}
STRUCT_END(HGrib1)

#ifndef __cplusplus
Message* read_message(Message* msg){

};
Message* next_message(Message* msg){

}
#endif