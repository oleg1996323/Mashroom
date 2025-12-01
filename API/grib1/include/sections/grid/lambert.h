#pragma once
#include "grid_base.h"

namespace grid{
    template<>
    struct GridDefinition<RepresentationType::LAMBERT>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition() = default;
        GridDefinition(unsigned char* buffer);
        GridDefinition(const GridDefinition& other):GridDefinitionBase(other){}
        GridDefinition(GridDefinition&& other) noexcept:GridDefinitionBase(std::move(other)){}
        GridDefinition& operator=(const GridDefinition& other){
            if(this!=&other){
                GridDefinitionBase::operator=(other);
            }
            return *this;
        }
        GridDefinition& operator=(GridDefinition&& other){
            if(this!=&other){
                GridDefinitionBase::operator=(std::move(other));
            }
            return *this;
        }
        std::string print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::LAMBERT>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };

    template<>
    struct GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition() = default;
        GridDefinition(unsigned char* buffer);
        GridDefinition(const GridDefinition& other):GridDefinitionBase(other){}
        GridDefinition(GridDefinition&& other) noexcept:GridDefinitionBase(std::move(other)){}
        GridDefinition& operator=(const GridDefinition& other){
            if(this!=&other){
                GridDefinitionBase::operator=(other);
            }
            return *this;
        }
        GridDefinition& operator=(GridDefinition&& other){
            if(this!=&other){
                GridDefinitionBase::operator=(std::move(other));
            }
            return *this;
        }
        std::string print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::dx),
                decltype(type::dy),decltype(type::is_south_pole),decltype(type::is_bipolar),decltype(type::scan_mode),decltype(type::latin1),decltype(type::latin2),decltype(type::latitude_south_pole),
                decltype(type::longitude_south_pole)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::dx),
                decltype(type::dy),decltype(type::is_south_pole),decltype(type::is_bipolar),decltype(type::scan_mode),decltype(type::latin1),decltype(type::latin2),decltype(type::latitude_south_pole),
                decltype(type::longitude_south_pole)>();
        }();
    };
}

template<>
std::expected<grid::GridBase<RepresentationType::LAMBERT>,std::exception> from_json<grid::GridBase<RepresentationType::LAMBERT>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<RepresentationType::LAMBERT>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<LAMBERT>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<LAMBERT>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<LAMBERT>>::value);