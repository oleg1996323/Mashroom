#pragma once
#include "AbstractDatum.h"

namespace datum{

class ITRF2008_2014 final:public EllipticDatum{
    static constexpr std::string_view name_ = "ITRF-2008/2014";
    public:
    ITRF2008_2014():
        EllipticDatum(
            name_,
            6378136.6,
            1./298.25642){}
};
}