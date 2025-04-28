#include "contains.h"
#include "data/msg.h"
#include "message.h"
#include "aux_code/int_pow.h"

bool contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    const CommonDataProperties& data,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        throw std::invalid_argument("Invalid position");
    if(!grib.open_grib(from))
        throw std::runtime_error("Unable to open file "s+from.c_str());
    do{
        const auto& msg = grib.message();
        if(!msg.has_value())
            throw std::runtime_error("Message undefined");
        GribMsgDataInfo info(	std::move(msg.value().get().section_2_.define_grid()),
                                    std::move(msg.value().get().section_1_.date()),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg.value().get().section_1_.IndicatorOfParameter(),
                                    msg.value().get().section_1_.unit_time_range(),
                                    msg.value().get().section_1_.center(),
                                    msg.value().get().section_1_.table_version());
        
    
    if(data.center_!=info.center)
        continue;
    if(data.table_version_!=info.table_version)
        continue;
    if(data.fcst_unit_!=info.t_unit)
        continue;
    if(data.parameter_!=info.parameter)
        continue;
    if(!grid_type.has_value() || grid_type.value()!=info.grid_data.value().rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data.value()))
        continue;
    }while(grib.next_message());
}

bool contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    Organization center, uint8_t table_version, uint8_t parameter,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        throw std::invalid_argument("Invalid position");
    if(!grib.open_grib(from))
        throw std::runtime_error("Unable to open file "s+from.c_str());
    do{
        const auto msg = grib.message();
        if(!msg.has_value())
            throw std::runtime_error("Message undefined");
        GribMsgDataInfo info(	std::move(msg.value().get().section_2_.define_grid()),
                                    std::move(msg.value().get().section_1_.date()),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg.value().get().section_1_.IndicatorOfParameter(),
                                    msg.value().get().section_1_.unit_time_range(),
                                    msg.value().get().section_1_.center(),
                                    msg.value().get().section_1_.table_version());
    if(center!=info.center)
        continue;
    if(table_version!=info.table_version)
        continue;
    if(parameter!=info.parameter)
        continue;
    if(!grid_type.has_value() || grid_type.value()!=info.grid_data.value().rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data.value()))
        continue;
    }while(grib.next_message());
}