#pragma once
#include "../grid_additional.h"
#include "../grid_base.h"
#include "../grid.h"
#include "json_add.h"
#include "boost_functional/json.h"

template<template<RepresentationType> class GRIDDEF,RepresentationType REP>
std::expected<GRIDDEF<REP>,std::exception> from_json(const boost::json::value& val){
    static_assert(std::is_same_v<GRIDDEF<REP>,grid::GridDefinition<REP>>,"invalid function call");
    if(val.is_object()){
        grid::GridDefinition<REP> result;
        using additional = grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridDefinition<REP>::modification()>;
        using base = grid::GridBase<grid::GridDefinition<REP>::type()>;
        if(val.as_object().contains("base"))
            if(auto base_tmp = from_json<base>(val.as_object().at("base"));base_tmp.has_value())
                result.base_ = std::move(base_tmp.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        if(val.as_object().contains("additional") && val.as_object().contains("mod") && val.as_object().at("mod").is_uint64()){
            if(auto add_res = from_json_grid_addition<REP>(val.as_object().at("additional"));add_res.has_value())
                result.additional_ = std::move(add_res.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridDefinition<REP>& val){
    boost::json::value result;
    auto& obj = result.as_object();
    obj["base"] = to_json(val.base_);
    obj["additional"] = to_json(val.additional_);
    return result;
}

template<RepresentationType REP>
std::expected<grid::GridDefinition<REP>,std::exception> from_json_grid_definition(const boost::json::value& val){
    return from_json<grid::GridDefinition,REP>(val);
}