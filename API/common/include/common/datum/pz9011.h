#pragma once
#include "AbstractDatum.h"

namespace datum{

class PZ9011 final:public EllipticDatum{
    static constexpr std::string_view name_ = "ПЗ-90.11";
    public:
    PZ9011():
        EllipticDatum(
            name_,
            6378136.,
            1./298.25784){}
};
}