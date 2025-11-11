#pragma once
#include "../grid_additional.h"
#include "../grid_base.h"
#include "boost_functional/json.h"

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<REP,grid::GridModification::NONE>& val){
    boost::json::object result;
    return result;
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<REP,grid::GridModification::ROTATION>& val){
    boost::json::object obj;
    obj["SP lat"].emplace_int64() = val.yp;
    obj["SP lat"].emplace_int64() = val.xp;
    obj["rot angle (ibmf)"].emplace_double() = val.ang;
    return obj;
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<REP,grid::GridModification::STRETCHING>& val){
    boost::json::value result = boost::json::object();
    auto& obj = result.as_object();
    obj["lat pole stretch, mm"].emplace_int64() = val.ysp;
    obj["lon pole stretch, mm"].emplace_int64() = val.xsp;
    obj["stretch factor (ibmf)"].emplace_double() = val.s_factor;
    return result;
}

template<template<RepresentationType REP,grid::GridModification MOD> class ADD,RepresentationType REP,grid::GridModification MOD>
std::expected<grid::GridAdditional<REP,MOD>,std::exception> from_json(const boost::json::value& val){
    static_assert(std::is_same_v<ADD<REP,MOD>,grid::GridAdditional<REP,MOD>>);
    grid::GridAdditional<REP,MOD> result;
    if(val.is_object()){
        const boost::json::object& obj = val.as_object();
        if constexpr (grid::GridDefinition<REP>::modification() == grid::GridModification::ROTATION){
            if(obj.contains("rotation") && obj.at("rotation").is_object()){
                const boost::json::object& in_obj = val.at("rotation").as_object();
                if(in_obj.contains("SP lat") && in_obj.at("SP lat").is_int64())
                    result.yp = in_obj.at("SP lat").as_int64();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
                if(in_obj.contains("SP lon") && in_obj.at("SP lon").is_int64())
                    result.yp = in_obj.at("SP lon").as_int64();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
                if(in_obj.contains("rot angle (ibmf)") && in_obj.at("rot angle (ibmf)").is_int64())
                    result.yp = in_obj.at("rot angle (ibmf)").as_double();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
            }
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        if constexpr(grid::GridDefinition<REP>::modification() == grid::GridModification::STRETCHING){
            if(obj.contains("stretching") && obj.at("stretching").is_object()){
                const boost::json::object& in_obj = val.at("stretching").as_object();
                if(in_obj.contains("lat pole stretch, mm") && in_obj.at("lat pole stretch, mm").is_int64())
                    result.ysp = in_obj.at("SP lat").as_int64();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
                if(in_obj.contains("lon pole stretch, mm") && in_obj.at("lon pole stretch, mm").is_int64())
                    result.xsp = in_obj.at("lon pole stretch, mm").as_int64();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
                if(in_obj.contains("stretch factor (ibmf)") && in_obj.at("stretch factor (ibmf)").is_double())
                    result.s_factor = in_obj.at("stretch factor (ibmf)").as_double();
                else return std::unexpected(std::invalid_argument("invalid JSON input"));
            }
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        if constexpr(grid::GridDefinition<REP>::modification() == grid::GridModification::ROTATION_STRETCHING){
            if(auto res_rot = from_json<grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION>>(obj);res_rot.has_value())
                result.rot_ = std::move(res_rot.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            if(auto res_str = from_json<grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING>>(obj);res_str.has_value())
                result.stretch_ = std::move(res_str.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
    return result;
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>& val){
    boost::json::object obj;
    auto& obj_tmp = obj["rotation"].emplace_object();
    {
        obj_tmp["SP lat"].emplace_int64() = val.rot_.yp;
        obj_tmp["SP lat"].emplace_int64() = val.rot_.xp;
        obj_tmp["rot angle (ibmf)"].emplace_double() = val.rot_.ang;
    }
    obj_tmp = obj["stretching"].emplace_object();
    {
        obj_tmp["lat pole stretch, mm"].emplace_int64() = val.stretch_.ysp;
        obj_tmp["lon pole stretch, mm"].emplace_int64() = val.stretch_.xsp;
        obj_tmp["stretch factor (ibmf)"].emplace_double() = val.stretch_.s_factor;
    }
    return obj;
}

template<RepresentationType REP>
std::expected<grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridDefinition<REP>::modification()>,std::exception> from_json_grid_addition(const boost::json::value& val){
    return from_json<grid::GridAdditional,grid::GridDefinition<REP>::type(),grid::GridDefinition<REP>::modification()>(val);
}