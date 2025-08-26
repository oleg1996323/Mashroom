#pragma once
#include "grib1_def.h"
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
#include "error_data.h"

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;

struct Message{
    IndicatorSection section_0_;
    ProductDefinitionSection section_1_;
    GridDescriptionSection section_2_;
    BitMapSection section_3_;
    BinaryDataSection section_4_;
    API::ErrorData::Code<API_TYPE>::value err_;

    Message(unsigned char* buffer):section_0_(buffer),
                                    section_1_(buffer+sec_0_min_sz),
                                    section_2_(section_1_.section1Flags().sec2_inc?section_1_.buffer_+section_1_.section_length():nullptr),
                                    section_3_(section_1_.section1Flags().sec3_inc?
                                                section_1_.buffer_+section_1_.section_length()+section_2_.section_length():nullptr),
                                    section_4_(section_1_.buffer_+section_1_.section_length()+section_2_.section_length()+section_3_.section_length())
    {
        // printf("Message begin pos: %p\n",buffer);
        // printf("Message section 1 pos: %p\n",buffer+sec_0_min_sz);
        // printf("Message section 2 pos: %p\n",(section_1_.section1Flags().sec2_inc?section_1_.buffer_+section_1_.section_length():nullptr));
        // printf("Message section 3 pos: %p\n",(section_1_.section1Flags().sec3_inc?(section_1_.section1Flags().sec2_inc?
        //                                     section_2_.buf_+section_2_.section_length():section_1_.buffer_+section_1_.section_length()):nullptr));
        // printf("Message section 4 pos: %p\n",(section3().has_value()?section_3_.buf_+section_3_.section_length():
        //                                         section2().has_value()?section_2_.buf_+section_2_.section_length():
        //                                         section_1_.buffer_+section_1_.section_length()));
        if(sec_0_min_sz+section_1_.section_length()+section_2_.section_length()+section_3_.section_length()+section_4_.get_BDS_length()+4!=message_length())
            err_ = API::ErrorData::Code<API_TYPE>::LEN_UNCONSIST_X1;
    }

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
};

struct HGrib1
{   
    private:
    std::unique_ptr<::Message> msg_ = nullptr;
    unsigned char* __f_ptr = nullptr;
    unsigned char* current_ptr_ = nullptr;
    unsigned long sz_ = 0;
    int file = -1;

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
    API::ErrorData::Code<API_TYPE>::value open_grib(const fs::path& filename);
    bool set_message(ptrdiff_t pos) noexcept;
};

#ifndef __cplusplus
Message* read_message(Message* msg){

};
Message* next_message(Message* msg){

}
#endif