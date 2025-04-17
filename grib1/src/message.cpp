#include "message.h"
#include "aux_code/int_pow.h"

//Bit 3 is set to 1 to indicate that the original data were integers; when this is the case any non-zero reference values should be rounded to an integer value prior to placing in the GRIB BDS
//Bit 4 is set to 1 to indicate that bits 5 to 12 are contained in octet 14 of the data section.
//Although GRIB is not capable of representing a matrix of data values at each grid point, the meaning of bit 6 is retained in anticipation of a future capability.
//When secondary bit maps are present in the data (used in association with second order packing) this is indicated by setting bit 7 to 1.
//When octet 14 contains the extended flag information octets 12 and 13 will also contain "special" information; the actual data will begin in a subsequent octet. See above.
float Message::extract_value(int n){
    double scale = int_power(10.0, - section_1_.decimal_scale_factor())*int_power(2.0, section_4_.scale_factor());
	Flag flags = section_4_.get_data_flag();
	if(!flags.complex_pack){
		if(!flags.spherical_harm_coefs){
			if(section_4_.bit_per_value()==0)
				return section_4_.ref_value();
			else{
                if(section_1_.section1Flags().sec3_inc){
                    int t_bits=0,mask_idx;
                    unsigned int tbits=0,jmask,bbits;
                    /* check bitmap */
                    unsigned char* bitmap = section_3_.buf_;
                    unsigned char* bds_bits = section_4_.buf_;
                    bitmap+=n-1;
                    /* check bitmap */
                    mask_idx = n & 7; //indicate the checking bit in the sequence of n bytes by 1
                    if (mask_idx == 0) bbits = *bitmap++; //if i=0, simply copy bitmap byte
                    //static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};
                    //static unsigned int map_masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};
                    //static double shift[9] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};
                    if ((bbits & map_masks[mask_idx]) == 0) { //if bitmap bit is 0, then in BDS value not presented and UNDEFINED must be returned
                        return UNDEFINED;
                    }

                    while (t_bits < section_4_.bit_per_value()) { //else do while all bytes not read in reserved bits for value in BDS
                        tbits = (tbits * 256) + *bds_bits++; //2^8 + byte value, then next byte of bds
                        t_bits += 8; //increment by byte
                    }
                    t_bits -= section_4_.bit_per_value();
                    return section_4_.ref_value() + scale*((tbits >> t_bits) & jmask);
                }
				else{
                    int t_bits = 0;
                    unsigned int tbits = 0, jmask = (1 << section_4_.bit_per_value()) - 1; //get filling by 1 in all bit per value field
                    unsigned char *bits = section_4_.buf_;
                    bits += 2*(n-1); //even offset to the n - value of datum
                    if (section_4_.bit_per_value()>8) {
                        tbits = (bits[0] << 8) | (bits[1]); //guaranteed (even) initialization of bits
                        bits += 2; //offset by 2 (bits read above)
                        t_bits += 16; //number read bits
                    }
                    while (t_bits<section_4_.bit_per_value()) { //if number of read bits is less than number of bits per value
                        tbits = (tbits * 256) + *bits++; //2^8 + next byte value
                        t_bits += 8; //increment by byte
                    }
                    t_bits -= section_4_.bit_per_value(); //rest of not read bytes
                    return section_4_.ref_value() + scale*((tbits >> t_bits) & jmask); //offset tbits to right and binary AND with all jmask 111...111 defined above
                }
			}
		}
	}
    return UNDEFINED;
}

std::vector<float> Message::extract_all(){
    Flag flags = section_4_.get_data_flag();
    std::vector<float> result;
	if(!flags.complex_pack){
		if(!flags.spherical_harm_coefs){
			if(section_4_.bit_per_value()==0)
				return {section_4_.ref_value()};
			else{
                if(section_1_.section1Flags().sec3_inc){
                    int t_bits=0,mask_idx;
                    unsigned int tbits=0,jmask,bbits;
                    /* check bitmap */
                    unsigned char* bitmap = section_3_.buf_;
                    unsigned char* bds_bits = section_4_.buf_;
                    result.resize(section_2_.nxny());
                    for(long i = 0;i<section_2_.nxny();++i){
                        bitmap+=i-1;
                        /* check bitmap */
                        mask_idx = i & 7; //indicate the checking bit in the sequence of n bytes by 1
                        if (mask_idx == 0) bbits = *bitmap++; //if i=0, simply copy bitmap byte
                        //static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};
                        //static unsigned int map_masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};
                        //static double shift[9] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};
                        if ((bbits & map_masks[mask_idx]) == 0) { //if bitmap bit is 0, then in BDS value not presented and UNDEFINED must be returned
                            result[i]=UNDEFINED;
                            continue;
                        }
                        while (t_bits < section_4_.bit_per_value()) { //else do while all bytes not read in reserved bits for value in BDS
                            tbits = (tbits * 256) + *bds_bits++; //2^8 + byte value, then next byte of bds
                            t_bits += 8; //increment by byte
                        }
                        t_bits -= section_4_.bit_per_value();
                        result[i] = section_4_.ref_value() + section_4_.scale_factor()*((tbits >> t_bits) & jmask);
                    }
                }
				else{
                    int t_bits = 0;
                    unsigned int tbits = 0, jmask = (1 << section_4_.bit_per_value()) - 1; //get filling by 1 in all bit per value field
                    unsigned char *bits = section_4_.buf_;
                    for(long i = 0;i<section_2_.nxny();++i){
                        if (section_4_.bit_per_value()-t_bits>8) {
                            tbits = (tbits << 16) | (bits[0] << 8) | (bits[1]); //guaranteed (even) initialization of bits
                            bits += 2; //offset by 2 (bits read above)
                            t_bits += 16; //number read bits
                        }
                        while (t_bits<section_4_.bit_per_value()) { //if number of read bits is less than number of bits per value
                            tbits = (tbits * 256) + *bits++; //2^8 + next byte value
                            t_bits += 8; //increment by byte
                        }
                        t_bits -= section_4_.bit_per_value(); //rest of not read bytes
                        result[i] = section_4_.ref_value() + section_4_.scale_factor()*((tbits >> t_bits) & jmask); //offset tbits to right and binary AND with all jmask 111...111 defined above
                    }
                }
                
			}
		}
	}
    return result;
}

std::optional<std::reference_wrapper<Message>> HGrib1::message() const{
    if(msg_)
        return *msg_;
    else return std::nullopt;
}
ptrdiff_t HGrib1::current_message_position() const noexcept{
    return static_cast<ptrdiff_t>(current_ptr_-__f_ptr);
}
std::optional<unsigned long> HGrib1::current_message_length() const noexcept{
    if(msg_)
        return msg_->section_0_.message_length();
    else return std::nullopt;
}
bool HGrib1::next_message(){
    if(msg_){
        if((current_ptr_-__f_ptr)+msg_->section_0_.message_length()<sz_){
            current_ptr_+=msg_->section_0_.message_length();
            msg_ = std::make_unique<Message>(current_ptr_);
            return true;
        }
        return false;
    }
    return false;
}
std::optional<unsigned long> HGrib1::file_size() const noexcept{
    if(msg_){
        return sz_;
    }
    return std::nullopt;
}
bool HGrib1::is_correct_format() const noexcept{
    if(msg_ && sz_>=sec_0_min_sz)
        if(memcmp(msg_->section_0_.buf_,"GRIB",4))
            return true;
    return false;
}
std::optional<unsigned char> HGrib1::grib_version() const noexcept{
    if(is_correct_format())
        return msg_->section_0_.grib_version();
}
ErrorCodeData HGrib1::open_grib(const fs::path& filename){
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
            std::cout<<current_message_length().value()<<std::endl;
            if(!memcmp(__f_ptr+current_message_length().value()-5,"7777",4))
                return ErrorCodeData::MISS_END_SECTION;
        }
        else return ErrorCodeData::DATA_EMPTY;
    }
    catch(...){
        return ErrorCodeData::BAD_FILE;
    }
    return ErrorCodeData::NONE_ERR;
}