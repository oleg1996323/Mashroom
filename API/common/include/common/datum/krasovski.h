#pragma once
#include "AbstractDatum.h"

namespace datum{

class Krasovsky final:public EllipticDatum{
    static constexpr std::string_view name_ = "Красовский";
    public:
    Krasovsky():
        EllipticDatum(
            name_,
            6378245.,
            1./298.3){}
};
}