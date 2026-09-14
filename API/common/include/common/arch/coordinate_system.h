#pragma once
#include <boost/geometry.hpp>

namespace geometry::system{
namespace detail{
    struct cs{};
}

struct cartesian final:detail::cs{
    struct strategy_distance{

    };
};
struct spherical final:detail::cs{
    struct strategy_distance{

    };
};
struct ellipsoidal final:detail::cs{
    struct strategy_distance{

    };
};
}