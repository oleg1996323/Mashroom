#pragma once
#include "AbstractDatum.h"

namespace datum{

class CGMS final:public EllipticDatum{
    static constexpr std::string_view name_ = "CGMS";
    public:
    CGMS():
        EllipticDatum(
            name_,
            6378169.,
            1.-6356583.8/6378169.){}
};

}