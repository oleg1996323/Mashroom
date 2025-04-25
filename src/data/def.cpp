#include "data/def.h"
#include <string.h>

DataTypeInfo extension_to_type(const char* extension) noexcept{
    for(int i=0;i<data_extensions.size();++i)
        if(strcmp(extension,data_extensions[i])==0)
            return DataTypeInfo(i);
    return DataTypeInfo::Undef;    
}

const char* type_to_extension(DataTypeInfo type_extension) noexcept{
    return data_extensions[(int)type_extension];
}

DataTypeInfo text_to_data_type(const char* text) noexcept{
    std::string_view str(text);
    if(str == "Grib")
        return DataTypeInfo::Grib;
    else return DataTypeInfo::Undef;
}

const char* data_type_to_text(DataTypeInfo type) noexcept{
    switch (type)
    {
    case DataTypeInfo::Grib:
        return "Grib";
        break;
    
    default:
        return "Undefined";
        break;
    }
}
using namespace std::string_literals;
std::string filename_by_type(DataTypeInfo type){
    return ""s+bindata_filename.data()+type_to_extension(type);
}