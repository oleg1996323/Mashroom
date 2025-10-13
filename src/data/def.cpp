#include "data/def.h"
#include <string.h>

__Data__::FORMAT extension_to_type(std::string_view extension) noexcept{
    for(int i=1;i<data_extensions.size();++i)
        if(extension==data_extensions[i])
            return static_cast<__Data__::FORMAT>(i);
    return __Data__::FORMAT::UNDEF;    
}

std::string_view type_to_extension(__Data__::FORMAT type_extension) noexcept{
    return data_extensions[(int)type_extension];
}

__Data__::FORMAT text_to_data_type(std::string_view text) noexcept{
    std::string_view str(text);
    if(str == "Grib")
        return __Data__::FORMAT::GRIB;
    else return __Data__::FORMAT::UNDEF;
}
using namespace std::string_literals;
std::string filename_by_type(__Data__::FORMAT type){
    return ""s+bindata_filename.data()+type_to_extension(type).data();
}