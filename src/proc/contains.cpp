#include "proc/contains.h"
#include "data/msg.h"
#include "message.h"
#include "int_pow.h"
#include <expected>

std::expected<bool,ErrorCode> contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    const CommonDataProperties& data,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        return std::unexpected(ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE));
    if(grib.open_grib(from)!=ErrorCodeData::NONE_ERR)
        return std::unexpected(ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,from.c_str()));
    do{
        const auto& msg = grib.message();
        if(!msg.has_value())
            return std::unexpected(ErrorPrint::print_error(ErrorCode::DATA_NOT_FOUND,"Message undefined",AT_ERROR_ACTION::CONTINUE));
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
    if(!grid_type.has_value() || grid_type.value()!=info.grid_data.rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data))
        continue;
    }while(grib.next_message());
}

std::expected<bool,ErrorCode> contains(const fs::path& from,const std::chrono::system_clock::time_point& date ,const Coord& coord,
    Organization center, uint8_t table_version, uint8_t parameter,std::optional<RepresentationType> grid_type){
    HGrib1 grib;
    if(!is_correct_pos(&coord))
        return std::unexpected(ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE));
    if(grib.open_grib(from)!=ErrorCodeData::NONE_ERR)
        return std::unexpected(ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,from.c_str()));
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
    if(!grid_type.has_value() || grid_type.value()!=info.grid_data.rep_type)
        continue;
    if(date!=info.date)
        continue;
    if(!pos_in_grid(coord,info.grid_data))
        continue;
    }while(grib.next_message());
}

/**
 * @brief Specify to check contents of only integral spacial and temporal data
 */
void Contains::set_integral_only(bool integral){
    integral_only_ = integral;
}