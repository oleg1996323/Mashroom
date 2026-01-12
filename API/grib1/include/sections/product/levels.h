#pragma once
#include "code_tables/table_3.h"
#include <stdexcept>
#include "serialization.h"

class Level{
    private:
    uint8_t octet_11 = 0;
    uint8_t octet_12 = 0;
    LevelsTags level_type_;
    template<bool,auto>
    friend struct serialization::Serialize;
    template<bool,auto>
    friend struct serialization::Deserialize;
    template<auto>
    friend struct serialization::Serial_size;
    template<auto>
    friend struct serialization::Min_serial_size;
    template<auto>
    friend struct serialization::Max_serial_size;
    friend struct std::hash<Level>;

    public:
    Level() = default;
    Level(LevelsTags tag,uint8_t o_11, uint8_t o_12):
        octet_11(o_11),octet_12(o_12),level_type_(tag){}
    Level(const Level& other){
        *this=other;
    }
    Level(Level&& other){
        *this=std::move(other);
    }
    Level& operator=(const Level& other){
        if(this!=&other){
            octet_11 = other.octet_11;
            octet_12 = other.octet_12;
            level_type_ = other.level_type_;
        }
        return *this;
    }
    Level& operator=(Level&& other){
        if(this!=&other){
            octet_11 = other.octet_11;
            octet_12 = other.octet_12;
            level_type_ = other.level_type_;
        }
        return *this;
    }
    void set_type(LevelsTags tag){
        level_type_ = tag;
    }
    void set_first_octet(uint8_t val){
        octet_11 = val;
    }
    void set_second_octet(uint8_t val){
        octet_12 = val;
    }
    uint8_t get_first_octet() const{
        return octet_11;
    }
    uint8_t get_second_octet() const{
        return octet_12;
    }
    LevelsTags type() const{
        return level_type_;
    }
    bool is_bounded() const{
        return levels_11_octets[level_type_]==2;
    }
    
    level_value_t get_top_bound() const{
        if(is_bounded())
            return convert_level<11>(level_type_,static_cast<uint16_t>(octet_11));
        else throw std::logic_error("level not bounded");
    }

    level_value_t get_bottom_bound() const{
        if(is_bounded())
            return convert_level<12>(level_type_,static_cast<uint16_t>(octet_12));
        else throw std::logic_error("level not bounded");
    }

    level_value_t get_level_height() const{
        if(!is_bounded())
            if(levels_11_octets[level_type_]==2)
                return convert_level<11>(level_type_,static_cast<uint16_t>(octet_11)<<8|octet_12);
            else return convert_level<11>(level_type_,static_cast<uint16_t>(octet_11));
        else throw std::logic_error("level bounded (expected boundaries request)");
    }

    bool operator==(const Level& other) const{
        return octet_11 == other.octet_11 && octet_12 == other.octet_12 && level_type_ == other.level_type_;
    }

    bool operator!=(const Level& other) const{
        return !(*this==other);
    }
};

template<>
struct std::hash<Level>{
    size_t operator()(const Level& val) const{
        return (static_cast<size_t>(val.type())<<16)|(static_cast<size_t>(val.octet_11)<<8)|(static_cast<size_t>(val.octet_12));
    }
};

#include "boost_functional/json.h"

template<>
std::expected<Level,std::exception> from_json(const boost::json::value& val);

template<>
boost::json::value to_json(const Level& val);

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,Level>{
        using type = Level;
        SerializationEC operator()(const type& val, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.level_type_,val.octet_11,val.octet_12);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,Level>{
        using type = Level;
        SerializationEC operator()(type& val, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.level_type_,val.octet_11,val.octet_12);
        }
    };

    template<>
    struct Serial_size<Level>{
        using type = Level;
        size_t operator()(const type& val) const noexcept{
            return serial_size(val.level_type_,val.octet_11,val.octet_12);
        }
    };

    template<>
    struct Min_serial_size<Level>{
        using type = Level;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::level_type_),decltype(type::octet_11),
            decltype(type::octet_12)>();
        }();
    };

    template<>
    struct Max_serial_size<Level>{
        using type = Level;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::level_type_),decltype(type::octet_11),
            decltype(type::octet_12)>();
        }();
    };
}

/* wesley ebisuzaki v1.0
 *
 * levels.c
 *
 * prints out a simple description of kpds6, kpds7
 *    (level/layer data)
 *  kpds6 = octet 10 of the PDS
 *  kpds7 = octet 11 and 12 of the PDS
 *    (kpds values are from NMC's grib routines)
 *  center = PDS_Center(pds) .. NMC, ECMWF, etc
 *
 * the description of the levels is 
 *   (1) incomplete
 *   (2) include some NMC-only values (>= 200?)
 *
 * v1.1 wgrib v1.7.3.1 updated with new levels
 * v1.2 added new level and new parameter
 * v1.2.1 modified level 117 pv units
 * v1.2.2 corrected level 141
 * v1.2.3 fixed layer 206 (was 205)
 * v1.2.4 layer 210: new wmo defn > NCEP version
 * v1.2.5 updated table 3/2007 to on388
 */
//@todo check and remove if necessary
void levels(int kpds6, int kpds7, int center, int verbose);