#pragma once
#include "AbstractDatum.h"

namespace datum{

class GSK2011 final:public EllipticDatum{
    static constexpr std::string_view name_ = "ГСК2011";
    public:
    GSK2011():
        EllipticDatum(
            name_,
            6378136.5,
            1./298.2564151){}
};

}