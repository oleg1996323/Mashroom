#pragma once
#include "AbstractDatum.h"

namespace datum{

class WGS84 final:public EllipticDatum{
    static constexpr std::string_view name_ = "WGS84";
    public:
    WGS84():
        EllipticDatum(
            name_,
            6378137.,
            1./298.257223563){}
};
}