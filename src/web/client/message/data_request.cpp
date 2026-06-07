#include "web/client/message/data_request.h"
ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>::ExtractRequestForm(const SearchProperties& search_props,
    std::optional<DateTimeDiff> t_sep,
    std::optional<OutputDataFileFormats> file_fmt):
    search_props_(search_props),t_separation_(t_sep),
    file_fmt_(file_fmt){}
ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>::ExtractRequestForm(SearchProperties&& search_props,
    std::optional<DateTimeDiff>&& t_sep,
    std::optional<OutputDataFileFormats> file_fmt):
    search_props_(std::move(search_props)),
    t_separation_(std::move(t_sep)),
    file_fmt_(file_fmt){}

ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>::ExtractRequestForm(ExtractRequestForm&& other) noexcept{
    *this = std::move(other);
}
ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>& 
ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>::operator=(ExtractRequestForm&& other) noexcept{
    if(this!=&other)
        search_props_ = std::move(other.search_props_);
    return *this;
}