#pragma once
#include <vector>
#include <span>
#include <bit>
#include "def.h"
#include "network/common/utility.h"
#include "grid_base.h"
#include "grid.h"
#include "functional/serialization.h"

using namespace network::utility;

namespace serialization{
    template<RepresentationType REP_T, bool NETWORK_ORDER = true>
    bool serialize(const GridBase<REP_T>& base, std::vector<char>& buf);

    template<RepresentationType REP_T,GridModification MOD, bool NETWORK_ORDER = true>
    bool serialize(const GridAdditional<REP_T,MOD>& addition,std::vector<char>& buf);

    template<RepresentationType REP_T, bool NETWORK_ORDER = true>
    bool serialize(const GridAdditional<REP_T,GridModification::NONE>& addition,std::vector<char>& buf){
		return;
	}

    template<bool NETWORK_ORDER = true>
    bool serialize<ScanMode,NETWORK_ORDER>(const ScanMode& scan_mode,std::vector<char>& buf){
        buf.insert(buf.end(),(const char*)&scan_mode,(const char*)(&scan_mode+sizeof(scan_mode)));
		return true;
	}

    template<>
    constexpr size_t serial_size(const ScanMode& mode){
        return sizeof(ScanMode);
    }

    template<>
    constexpr size_t min_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(ScanMode);
    }

    template<>
    constexpr size_t max_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(ScanMode);
    }

    template<bool NETWORK_ORDER = true>
    bool serialize<ResolutionComponentFlags,NETWORK_ORDER>(const ResolutionComponentFlags& res_flags,std::vector<char>& buf){
        buf.insert(buf.end(),(const char*)&res_flags,(const char*)(&res_flags+sizeof(res_flags)));
		return true;
	}

    template<>
    constexpr size_t serial_size(const ResolutionComponentFlags& mode){
        return sizeof(ResolutionComponentFlags);
    }

    template<>
    constexpr size_t min_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(ResolutionComponentFlags);
    }

    template<>
    constexpr size_t max_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(ResolutionComponentFlags);
    }

    template<bool NETWORK_ORDER = true>
    bool serialize(const Spectral& spectral,std::vector<char>& buf){
        buf.insert(buf.end(),(const char*)&spectral,(const char*)(&spectral+sizeof(spectral)));
		return true;
	}

    template<>
    constexpr size_t serial_size(const Spectral& mode){
        return sizeof(Spectral);
    }

    template<>
    constexpr size_t min_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(Spectral);
    }

    template<>
    constexpr size_t max_serial_size(const ResolutionComponentFlags& mode){
        return sizeof(Spectral);
    }

    template<RepresentationType REP_T,bool NETWORK_ORDER = true>
    bool serialize(const GridAdditional<REP_T,GridModification::ROTATION>& addition,std::vector<char>& buf){
		using namespace network::utility;
        serialize<decltype(addition.yp),NETWORK_ORDER>(addition.yp);
        serialize<decltype(addition.xp),NETWORK_ORDER>(addition.xp);
        serialize<decltype(addition.ang),NETWORK_ORDER>(addition.ang);
	}

    template<RepresentationType REP_T,bool NETWORK_ORDER = true>
    bool serialize(const GridAdditional<REP_T,GridModification::STRETCHING>& addition,std::vector<char>& buf){
		using namespace network::utility;
        serialize<decltype(addition.ysp),NETWORK_ORDER>(addition.ysp);
        serialize<decltype(addition.xsp),NETWORK_ORDER>(addition.xsp);
        serialize<decltype(addition.s_factor),NETWORK_ORDER>(addition.s_factor);
	}

    template<RepresentationType REP_T,bool NETWORK_ORDER = true>
    bool serialize(const GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>& addition,std::vector<char>& buf){
		serialize<REP_T,NETWORK_ORDER>(addition.rot_,buf);
        serialize<REP_T,NETWORK_ORDER>(addition.stretch_,buf);
	}

    template<bool NETWORK_ORDER = true>
    bool serialize<GAUSSIAN,NETWORK_ORDER>(const GridBase<GAUSSIAN>& base, std::vector<char>& buf){
        serialize<decltype(base.y1,buf),NETWORK_ORDER>(base.y1,buf);
        serialize<decltype(base.x1,buf),NETWORK_ORDER>(base.x1,buf);
        serialize<decltype(base.y2,buf),NETWORK_ORDER>(base.y2,buf);
        serialize<decltype(base.x2,buf),NETWORK_ORDER>(base.x2,buf);
        serialize<decltype(base.ny,buf),NETWORK_ORDER>(base.ny,buf);
        serialize<decltype(base.nx,buf),NETWORK_ORDER>(base.nx,buf);
        serialize<decltype(base.directionIncrement,buf),NETWORK_ORDER>(base.directionIncrement,buf);
        serialize<decltype(base.N,buf),NETWORK_ORDER>(base.N,buf);
        serialize<decltype(base.scan_mode,buf),NETWORK_ORDER>(base.scan_mode,buf);
        serialize<decltype(base.resolutionAndComponentFlags,buf),NETWORK_ORDER>(base.resolutionAndComponentFlags,buf);
    }

    template<bool NETWORK_ORDER = true>
    bool serialize<ALBERS_EQUAL_AREA,NETWORK_ORDER>(const GridBase<ALBERS_EQUAL_AREA>& base,std::vector<char>& buf){
        serialize<decltype(base.y1,buf),NETWORK_ORDER>(base.y1,buf);
        serialize<decltype(base.x1,buf),NETWORK_ORDER>(base.x1,buf);
        serialize<decltype(base.LoV,buf),NETWORK_ORDER>(base.LoV,buf);
        serialize<decltype(base.latin1,buf),NETWORK_ORDER>(base.latin1,buf);
        serialize<decltype(base.latin2,buf),NETWORK_ORDER>(base.latin2,buf);
        serialize<decltype(base.Dy,buf),NETWORK_ORDER>(base.Dy,buf);
        serialize<decltype(base.Dx,buf),NETWORK_ORDER>(base.Dx,buf);
        serialize<decltype(base.latitude_south_pole,buf),NETWORK_ORDER>(base.latitude_south_pole,buf);
        serialize<decltype(base.longitude_south_pole,buf),NETWORK_ORDER>(base.longitude_south_pole,buf);
        serialize<decltype(base.ny,buf),NETWORK_ORDER>(base.ny,buf);
        serialize<decltype(base.nx,buf),NETWORK_ORDER>(base.nx,buf);
        serialize<decltype(base.is_south_pole,buf),NETWORK_ORDER>(base.is_south_pole,buf);
        serialize<decltype(base.is_bipolar,buf),NETWORK_ORDER>(base.is_bipolar,buf);
        serialize<decltype(base.scan_mode,buf),NETWORK_ORDER>(base.scan_mode,buf);
        serialize<decltype(base.resolutionAndComponentFlags,buf),NETWORK_ORDER>(base.resolutionAndComponentFlags,buf);
    }

    template<bool NETWORK_ORDER = true>
    bool serialize<SPHERICAL_HARMONIC_COEFFICIENTS>(const GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>& base,std::vector<char>& buf){
        serialize<decltype(base.J,buf),NETWORK_ORDER>(base.J,buf);
        serialize<decltype(base.K,buf),NETWORK_ORDER>(base.K,buf);
        serialize<decltype(base.M,buf),NETWORK_ORDER>(base.M,buf);
        serialize<decltype(base.representation_type,buf),NETWORK_ORDER>(base.representation_type,buf);
        serialize<decltype(base.rep_mode,buf),NETWORK_ORDER>(base.rep_mode,buf);
    }

    template<RepresentationType REP_T, bool NETWORK_ORDER = true>
    auto deserialize(std::span<const char> buf) noexcept->std::expected<GridBase<REP_T>,SerializationEC>;
    
    template<RepresentationType REP_T,GridModification MOD, bool NETWORK_ORDER = true>
    auto deserialize(std::span<const char> buf) noexcept->std::expected<GridAdditional<REP_T,MOD>,SerializationEC>;

    template<RepresentationType REP_T, bool NETWORK_ORDER = true>
    auto deserialize<REP_T,GridModification::NONE,NETWORK_ORDER>(std::span<const char> buf) noexcept
            ->std::expected<GridAdditional<REP_T,GridModification::NONE>,SerializationEC>
    {
		return GridAdditional<REP_T,GridModification::NONE>();
	}

    template<RepresentationType REP_T, bool NETWORK_ORDER>
    auto deserialize<REP_T,GridModification::STRETCHING,NETWORK_ORDER>(std::span<const char> buf) noexcept
            ->std::expected<GridAdditional<REP_T,GridModification::STRETCHING>,SerializationEC>
    {
		using namespace network::utility;
            if(buf.size()<GridAdditional<REP_T,GridModification::STRETCHING>::serial_size())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        GridAdditional<REP_T,GridModification::STRETCHING> addition;
            auto ysp_result = deserialize<decltype(addition.ysp),NETWORK_ORDER>(buf);
            if(!ysp_result)
                return std::unexpected(ysp_result.error());
            else addition.ysp = *ysp_result;
            auto xsp_result = deserialize<decltype(addition.xsp),NETWORK_ORDER>(buf = buf.subspan(serial_size(addition.ysp)));
            if(!xsp_result)
                return std::unexpected(xsp_result.error());
            else addition.xsp = *xsp_result;
			auto factor_result = deserialize<decltype(addition.s_factor),NETWORK_ORDER>(buf = buf.subspan(serial_size(addition.xsp)));
            if(!factor_result)
                return std::unexpected(factor_result.error());
            else addition.s_factor = *factor_result;
		// else{
        //     auto ysp_result = deserialize_native<decltype(addition.ysp)>(buf);
        //     if(!ysp_result)
        //         return std::unexpected(ysp_result.error());
        //     else addition.ysp = *ysp_result;
        //     auto xsp_result = deserialize_native<decltype(addition.xsp),NETWORK_ORDER>(buf = buf.subspan(serial_size(addition.ysp)));
        //     if(!xsp_result)
        //         return std::unexpected(xsp_result.error());
        //     else addition.xsp = *xsp_result;
		// 	auto factor_result = deserialize_native<decltype(addition.s_factor)>(buf = buf.subspan(serial_size(addition.xsp)));
        //     if(!factor_result)
        //         return std::unexpected(factor_result.error());
        //     else addition.s_factor = *factor_result;
		// }
        return addition;
	}

    template<RepresentationType REP_T, bool NETWORK_ORDER>
    auto deserialize<REP_T,GridModification::ROTATION,NETWORK_ORDER>(std::span<const char> buf) noexcept
            ->std::expected<GridAdditional<REP_T,GridModification::ROTATION>,SerializationEC>
    {
		using namespace network::utility;
        if(buf.size()<GridAdditional<REP_T,GridModification::ROTATION>::serial_size())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        GridAdditional<REP_T,GridModification::ROTATION> addition;
		uint16_t offset = GridBase<REP_T>::serial_size();
		//if(is_little_endian()){
            auto yp_result = deserialize<decltype(addition.yp),NETWORK_ORDER>(buf);
            if(!yp_result)
                return std::unexpected(yp_result.error());
            else addition.yp = *yp_result;
            auto xp_result = deserialize<decltype(addition.xp),NETWORK_ORDER>(buf = buf.subspan(serial_size(addition.yp)));
            if(!xp_result)
                return std::unexpected(xp_result.error());
            else addition.xp = *xp_result;
			auto ang_result = deserialize<decltype(addition.ang),NETWORK_ORDER>(buf = buf.subspan(serial_size(addition.xp)));
            if(!ang_result)
                return std::unexpected(ang_result.error());
            else addition.ang = *ang_result;
		// }
		// else{
        //     auto yp_result = deserialize_native<decltype(addition.yp)>(buf);
        //     if(!yp_result)
        //         return std::unexpected(yp_result.error());
        //     else addition.yp = *yp_result;
        //     auto xp_result = deserialize_native<decltype(addition.xp)>(buf = buf.subspan(serial_size(addition.yp)));
        //     if(!xp_result)
        //         return std::unexpected(xp_result.error());
        //     else addition.xp = *xp_result;
		// 	auto ang_result = deserialize_native<decltype(addition.ang)>(buf = buf.subspan(serial_size(addition.xp)));
        //     if(!ang_result)
        //         return std::unexpected(ang_result.error());
        //     else addition.ang = *ang_result;
		// }
        return addition;
	}

    template<RepresentationType REP_T, bool NETWORK_ORDER = true>
    auto deserialize<REP_T,GridModification::ROTATION_STRETCHING,NETWORK_ORDER>(std::span<const char> buf) noexcept
            ->std::expected<GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>,SerializationEC>
    {
		using namespace network::utility;
        if(buf.size()<GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>::serial_size())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        GridAdditional<REP_T,GridModification::ROTATION_STRETCHING> addition;
        auto rot_result = deserialize<REP_T,GridModification::ROTATION,NETWORK_ORDER>(buf);
        if(!rot_result)
            return std:unexpected(rot_result.error());
        else addition.rot_ = *rot_result;
        auto stretch_result = deserialize<REP_T,GridModification::STRETCHING,NETWORK_ORDER>(buf);
        if(!stretch_result)
            return std:unexpected(stretch_result.error());
        else addition.stretch_ = *stretch_result;
        return addition;
	}

    template<bool NETWORK_ORDER = true>
    auto deserialize<GAUSSIAN,NETWORK_ORDER>(std::span<const char> buf) noexcept
        ->std::expected<GridBase<GAUSSIAN>,SerializationEC>
    {
		if(buf.size()!=GridBase<GAUSSIAN>::serial_size())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        else{
            GridBase<GAUSSIAN> base;
            uint16_t offset = 0;
            {
                auto y1_res = deserialize<decltype(base.y1),NETWORK_ORDER>(buf.subspan(offset));
                if(!y1_res)
                    return std::unexpected(y1_res.error());
                else base.y1 = *y1_res;
                offset+=serial_size(base.y1);
            }
            {
                auto x1_res = deserialize<decltype(base.x1),NETWORK_ORDER>(buf.subspan(offset));
                if(!x1_res)
                    return std::unexpected(x1_res.error());
                else base.x1 = *x1_res;
                offset+=serial_size(base.x1);
            }
            {
                auto y2_res = deserialize<decltype(base.y2),NETWORK_ORDER>(buf.subspan(offset));
                if(!y2_res)
                    return std::unexpected(y2_res.error());
                else base.y2 = *y2_res;
                offset+=serial_size(base.y2);
            }
            {
                auto x2_res = deserialize<decltype(base.x2),NETWORK_ORDER>(buf.subspan(offset));
                if(!x2_res)
                    return std::unexpected(x2_res.error());
                else base.x2 = *x2_res;
                offset+=serial_size(base.x2);
            }
            {
                auto ny_res = deserialize<decltype(base.ny),NETWORK_ORDER>(buf.subspan(offset));
                if(!ny_res)
                    return std::unexpected(ny_res.error());
                else base.ny = *ny_res;
                offset+=serial_size(base.ny);
            }
            {
                auto nx_res = deserialize<decltype(base.nx),NETWORK_ORDER>(buf.subspan(offset));
                if(!nx_res)
                    return std::unexpected(nx_res.error());
                else base.nx = *nx_res;
                offset+=serial_size(base.nx);
            }
            {
                auto dir_res = deserialize<decltype(base.directionIncrement),NETWORK_ORDER>(buf.subspan(offset));
                if(!dir_res)
                    return std::unexpected(dir_res.error());
                else base.directionIncrement = *dir_res;
                offset+=serial_size(base.directionIncrement);
            }
            {
                auto N_res = deserialize<decltype(base.N),NETWORK_ORDER>(buf.subspan(offset));
                if(!N_res)
                    return std::unexpected(N_res.error());
                else base.N = *N_res;
                offset+=serial_size(base.N);
            }
            {
                auto scan_res = deserialize<decltype(base.scan_mode),NETWORK_ORDER>(buf.subspan(offset));
                if(!scan_res)
                    return std::unexpected(scan_res.error());
                else base.scan_mode = *scan_res;
                offset+=serial_size(base.scan_mode);
            }
            {
                auto res_res = deserialize<decltype(base.resolutionAndComponentFlags),NETWORK_ORDER>(buf.subspan(offset));
                if(!res_res)
                    return std::unexpected(res_res.error());
                else base.resolutionAndComponentFlags = *res_res;
            }
            return base;
        }
	}
    template<>
    auto deserialize<ALBERS_EQUAL_AREA>(std::span<const char> buf) noexcept -> std::expected<GridBase<ALBERS_EQUAL_AREA>,SerializationEC>{
        if(buf.size()!=GridBase<ALBERS_EQUAL_AREA>::serial_size())
            return;
        else{
            GridBase<ALBERS_EQUAL_AREA> base;
            uint16_t offset = 0;
            if(is_little_endian()){
                {
                    auto y1_res = deserialize_network<decltype(base.y1)>(buf);
                    if(!y1_res)
                        return std::unexpected(y1_res.error());
                    else base.y1 = *y1_res;
                    offset+=serial_size(base.y1);
                }
                {
                    auto x1_res = deserialize_network<decltype(base.x1)>(buf.subspan(offset));
                    if(!x1_res)
                        return std::unexpected(x1_res.error());
                    else base.x1 = *x1_res;
                    offset+=serial_size(base.x1);
                }
                {
                    auto LoV_res = deserialize_network<decltype(base.LoV)>(buf.subspan(offset));
                    if(!LoV_res)
                        return std::unexpected(LoV_res.error());
                    else base.LoV = *LoV_res;
                    offset+=serial_size(base.LoV);
                }
                {
                    auto latin1_res = deserialize_network<decltype(base.latin1)>(buf.subspan(offset));
                    if(!latin1_res)
                        return std::unexpected(latin1_res.error());
                    else base.latin1 = *latin1_res;
                    offset+=serial_size(base.latin1);
                }
                {
                    auto latin2_res = deserialize_network<decltype(base.latin2)>(buf.subspan(offset));
                    if(!latin2_res)
                        return std::unexpected(latin2_res.error());
                    else base.latin2 = *latin2_res;
                    offset+=serial_size(base.latin2);
                }
                {
                    auto Dy_res = deserialize_network<decltype(base.Dy)>(buf.subspan(offset));
                    if(!Dy_res)
                        return std::unexpected(Dy_res.error());
                    else base.Dy = *Dy_res;
                    offset+=serial_size(base.Dy);
                }
                {
                    auto Dx_res = deserialize_network<decltype(base.Dx)>(buf.subspan(offset));
                    if(!Dx_res)
                        return std::unexpected(Dx_res.error());
                    else base.Dx = *Dx_res;
                    offset+=serial_size(base.Dx);
                }
                {
                    auto la_sp_res = deserialize_network<decltype(base.latitude_south_pole)>(buf.subspan(offset));
                    if(!la_sp_res)
                        return std::unexpected(la_sp_res.error());
                    else base.latitude_south_pole = *la_sp_res;
                    offset+=serial_size(base.latitude_south_pole);
                }
                {
                    auto lo_sp_res = deserialize_network<decltype(base.longitude_south_pole)>(buf.subspan(offset));
                    if(!lo_sp_res)
                        return std::unexpected(lo_sp_res.error());
                    else base.longitude_south_pole = *lo_sp_res;
                    offset+=serial_size(base.longitude_south_pole);
                }
                {
                    auto ny_res = deserialize_network<decltype(base.ny)>(buf.subspan(offset));
                    if(!ny_res)
                        return std::unexpected(ny_res.error());
                    else base.ny = *ny_res;
                    offset+=serial_size(base.ny);
                }
                {
                    auto nx_res = deserialize_network<decltype(base.nx)>(buf.subspan(offset));
                    if(!nx_res)
                        return std::unexpected(nx_res.error());
                    else base.nx = *nx_res;
                    offset+=serial_size(base.nx);
                }
                {
                    auto is_sp_res = deserialize_network<decltype(base.is_south_pole)>(buf.subspan(offset));
                    if(!is_sp_res)
                        return std::unexpected(is_sp_res.error());
                    else base.is_south_pole = *is_sp_res;
                    offset+=serial_size(base.is_south_pole);
                }
                {
                    auto is_bp_res = deserialize_network<decltype(base.is_bipolar)>(buf.subspan(offset));
                    if(!is_bp_res)
                        return std::unexpected(is_bp_res.error());
                    else base.is_bipolar = *is_bp_res;
                    offset+=serial_size(base.is_bipolar);
                }
                {
                    auto scan_res = deserialize_network<decltype(base.scan_mode)>(buf.subspan(offset));
                    if(!scan_res)
                        return std::unexpected(scan_res.error());
                    else base.scan_mode = *scan_res;
                    offset+=serial_size(base.scan_mode);
                }
                {
                    auto res_res = deserialize_network<decltype(base.resolutionAndComponentFlags)>(buf.subspan(offset));
                    if(!res_res)
                        return std::unexpected(res_res.error());
                    else base.resolutionAndComponentFlags = *res_res;
                }
                base.y1 = ntohf(*(decltype(base.y1)*)(buf.data()+offset));
                offset+=sizeof(base.y1);
                base.x1 = ntohf(*(decltype(base.x1)*)(buf.data()+offset));
                offset+=sizeof(base.x1);
                base.LoV = ntohf(*(decltype(base.LoV)*)(buf.data()+offset));
                offset+=sizeof(base.LoV);
                base.latin1 = ntohf(*(decltype(base.latin1)*)(buf.data()+offset));
                offset+=sizeof(base.latin1);
                base.latin2 = ntohf(*(decltype(base.latin2)*)(buf.data()+offset));
                offset+=sizeof(base.ny);
                base.Dy = ntohf(*(decltype(base.Dy)*)(buf.data()+offset));
                offset+=sizeof(base.Dy);
                base.Dx = ntohf(*(decltype(base.Dx)*)(buf.data()+offset));
                offset+=sizeof(base.Dx);
                base.latitude_south_pole = ntohf(*(decltype(base.latitude_south_pole)*)(buf.data()+offset));
                offset+=sizeof(base.latitude_south_pole);
                base.longitude_south_pole = ntohf(*(decltype(base.longitude_south_pole)*)(buf.data()+offset));
                offset+=sizeof(base.longitude_south_pole);
                base.ny = ntohs(*(decltype(base.ny)*)(buf.data()+offset));
                offset+=sizeof(base.ny);
                base.nx = ntohs(*(decltype(base.nx)*)(buf.data()+offset));
                offset+=sizeof(base.nx);
                base.is_south_pole = *(decltype(base.is_south_pole)*)(buf.data()+offset);
                offset+=sizeof(base.is_south_pole);
                base.is_bipolar = *(decltype(base.is_bipolar)*)(buf.data()+offset);
                offset+=sizeof(base.is_bipolar);
                base.scan_mode = *(ScanMode*)(buf.data()+offset);
                offset+=sizeof(ScanMode);
                base.resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
            }
            else{
                base.y1 = *(decltype(base.y1)*)(buf.data()+offset);
                offset+=sizeof(base.y1);
                base.x1 = *(decltype(base.x1)*)(buf.data()+offset);
                offset+=sizeof(base.x1);
                base.LoV = *(decltype(base.LoV)*)(buf.data()+offset);
                offset+=sizeof(base.LoV);
                base.latin1 = *(decltype(base.latin1)*)(buf.data()+offset);
                offset+=sizeof(base.latin1);
                base.latin2 = *(decltype(base.latin2)*)(buf.data()+offset);
                offset+=sizeof(base.ny);
                base.Dy = *(decltype(base.Dy)*)(buf.data()+offset);
                offset+=sizeof(base.Dy);
                base.Dx = *(decltype(base.Dx)*)(buf.data()+offset);
                offset+=sizeof(base.Dx);
                base.latitude_south_pole = *(decltype(base.latitude_south_pole)*)(buf.data()+offset);
                offset+=sizeof(base.latitude_south_pole);
                base.longitude_south_pole = *(decltype(base.longitude_south_pole)*)(buf.data()+offset);
                offset+=sizeof(base.longitude_south_pole);
                base.ny = *(decltype(base.ny)*)(buf.data()+offset);
                offset+=sizeof(base.ny);
                base.nx = *(decltype(base.nx)*)(buf.data()+offset);
                offset+=sizeof(base.nx);
                base.is_south_pole = *(decltype(base.is_south_pole)*)(buf.data()+offset);
                offset+=sizeof(base.is_south_pole);
                base.is_bipolar = *(decltype(base.is_bipolar)*)(buf.data()+offset);
                offset+=sizeof(base.is_bipolar);
                base.scan_mode = *(ScanMode*)(buf.data()+offset);
                offset+=sizeof(ScanMode);
                base.resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
            }
            return base;
        }
    }
    template<>
    auto deserialize<SPHERICAL_HARMONIC_COEFFICIENTS>(std::span<const char> buf) noexcept -> std::expected<GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>,SerializationEC>{
        if(buf.size()!=GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        else{
            GridBase<SPHERICAL_HARMONIC_COEFFICIENTS> base;
            uint16_t offset = 0;
            if(is_little_endian()){
                base.J = ntohs(*(decltype(base.J)*)(buf.data()+offset));
                offset+=sizeof(base.J);
                base.K = ntohs(*(decltype(base.K)*)(buf.data()+offset));
                offset+=sizeof(base.K);
                base.M = ntohs(*(decltype(base.M)*)(buf.data()+offset));
                offset+=sizeof(base.M);
                base.representation_type = *(decltype(base.representation_type)*)(buf.data()+offset);
                offset+=sizeof(base.representation_type);
                base.rep_mode = *(decltype(base.rep_mode)*)(buf.data()+offset);
            }
            else{
                base.J = *(decltype(base.J)*)(buf.data()+offset);
                offset+=sizeof(base.J);
                base.K = *(decltype(base.K)*)(buf.data()+offset);
                offset+=sizeof(base.K);
                base.M = *(decltype(base.M)*)(buf.data()+offset);
                offset+=sizeof(base.M);
                base.representation_type = *(decltype(base.representation_type)*)(buf.data()+offset);
                offset+=sizeof(base.representation_type);
                base.rep_mode = *(decltype(base.rep_mode)*)(buf.data()+offset);
            }
            return base;
        }
    }

    template<bool NETWORK_ORDER>
    bool serialize<GridInfo,NETWORK_ORDER>(const GridInfo& grid,std::vector<char>& buf){
        switch(rep_type){
        case RepresentationType::ALBERS_EQUAL_AREA :
            return serialize(grid.data.albers,buf);
            break;
        case RepresentationType::GAUSSIAN :
            return serialize(grid.data.gauss,buf);
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON :
            return serialize(grid.data.rot_gauss,buf);
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON :
            return serialize(grid.data.str_gauss,buf);
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON :
            return serialize(grid.data.str_rot_gauss,buf);
            break;
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR :
            return serialize(grid.data.latlon,buf);
            break;
        case RepresentationType::ROTATED_LAT_LON:
            return serialize(grid.data.rot_latlon,buf);
            break;
        case RepresentationType::STRETCHED_LAT_LON :
            return serialize(grid.data.str_latlon,buf);
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON :
            return serialize(grid.data.str_rot_latlon,buf);
            break;
        case RepresentationType::LAMBERT :
            return serialize(grid.data.lambert,buf);
            break;
        case RepresentationType::MERCATOR :
            return serialize(grid.data.mercator,buf);
            break;
        case RepresentationType::MILLERS_CYLINDR :
            return serialize(grid.data.millers,buf);
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL :
            return serialize(grid.data.lambert_oblique,buf);
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ :
            return serialize(grid.data.polar,buf);
            break;
        case RepresentationType::SIMPLE_POLYCONIC :
            return serialize(grid.data.polyconic,buf);
            break;
        case RepresentationType::GNOMONIC :
            return serialize(grid.data.gnomonic,buf);
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
            return serialize(grid.data.rot_harmonic,buf);
            break;
        case RepresentationType::UTM :
            return serialize(grid.data.utm,buf);
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS :
            return serialize(grid.data.str_harmonic,buf);
            break;
        case RepresentationType::SPACE_VIEW :
            return serialize(grid.data.space,buf);
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS :
            return serialize(grid.data.harmonic,buf);
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS :
            return serialize(grid.data.str_rot_harmonic,buf);
            break;
        default:
            return "Unknown serialize(";
            break;
        }
    }
}