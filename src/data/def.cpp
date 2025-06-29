#include "data/def.h"
#include <string.h>

__Data__::FORMAT extension_to_type(const char* extension) noexcept{
    for(int i=0;i<data_extensions.size();++i)
        if(strcmp(extension,data_extensions[i])==0)
            return static_cast<__Data__::FORMAT>(i+1);
    return __Data__::FORMAT::UNDEF;    
}

const char* type_to_extension(__Data__::FORMAT type_extension) noexcept{
    return data_extensions[(int)type_extension];
}

__Data__::FORMAT text_to_data_type(const char* text) noexcept{
    std::string_view str(text);
    if(str == "Grib")
        return __Data__::FORMAT::GRIB;
    else return __Data__::FORMAT::UNDEF;
}

const char* data_type_to_text(__Data__::FORMAT type) noexcept{
    switch (type)
    {
    case __Data__::FORMAT::GRIB:
        return "Grib";
        break;
    
    default:
        return "Undefined";
        break;
    }
}
using namespace std::string_literals;
std::string filename_by_type(__Data__::FORMAT type){
    return ""s+bindata_filename.data()+type_to_extension(type);
}