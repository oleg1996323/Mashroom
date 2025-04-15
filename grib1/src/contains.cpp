#include "contains.h"
#include "types/data_msg.h"
#include "message.h"
#include "aux_code/int_pow.h"

bool contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
                std::optional<RepresentationType> grid_type,
                std::optional<Organization> center,
                std::optional<uint8_t> subcenter,
                std::optional<TimeFrame> fcst){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        throw std::invalid_argument("Invalid position");
	if(!grib.open_grib(from))
        throw std::runtime_error("Unable to open file "s+from.c_str());
    do{
		GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
									std::move(grib.message().section_1_.date()),
									grib.current_message_position(),
									grib.current_message_length(),
									grib.message().section_1_.IndicatorOfParameter(),
									grib.message().section_1_.unit_time_range(),
									grib.message().section_1_.center(),
									grib.message().section_1_.subcenter());
    if(grid_type.has_value() && grid_type.value()!=info.grid_data.rep_type)
        continue;
    if(center.has_value() && center.value()!=info.center)
        continue;
    if(subcenter.has_value() && subcenter.value()!=info.subcenter)
        continue;
    if(fcst.has_value() && fcst.value()!=info.t_unit)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data))
        continue;
	}while(grib.next_message());
}

bool contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    const CommonDataProperties& data,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        throw std::invalid_argument("Invalid position");
    if(!grib.open_grib(from))
        throw std::runtime_error("Unable to open file "s+from.c_str());
    do{
        GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
                                    std::move(grib.message().section_1_.date()),
                                    grib.current_message_position(),
                                    grib.current_message_length(),
                                    grib.message().section_1_.IndicatorOfParameter(),
                                    grib.message().section_1_.unit_time_range(),
                                    grib.message().section_1_.center(),
                                    grib.message().section_1_.subcenter());
        
    
    if(data.center_!=info.center)
        continue;
    if(data.subcenter_!=info.subcenter)
        continue;
    if(data.fcst_unit_!=info.t_unit)
        continue;
    if(data.parameter_!=info.parameter)
        continue;
    if(grid_type.has_value() && grid_type.value()!=info.grid_data.rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data))
        continue;
    }while(grib.next_message());
}

bool contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    Organization center, uint8_t subcenter, uint8_t parameter,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        throw std::invalid_argument("Invalid position");
    if(!grib.open_grib(from))
        throw std::runtime_error("Unable to open file "s+from.c_str());
    do{
        GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
                                    std::move(grib.message().section_1_.date()),
                                    grib.current_message_position(),
                                    grib.current_message_length(),
                                    grib.message().section_1_.IndicatorOfParameter(),
                                    grib.message().section_1_.unit_time_range(),
                                    grib.message().section_1_.center(),
                                    grib.message().section_1_.subcenter());
    if(center!=info.center)
        continue;
    if(subcenter!=info.subcenter)
        continue;
    if(parameter!=info.parameter)
        continue;
    if(grid_type.has_value() && grid_type.value()!=info.grid_data.rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data))
        continue;
    }while(grib.next_message());
}