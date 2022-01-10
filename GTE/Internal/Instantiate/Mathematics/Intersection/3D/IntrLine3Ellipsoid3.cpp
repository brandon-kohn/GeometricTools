#include <Mathematics/IntrLine3Ellipsoid3.h>

namespace gte
{
    template class TIQuery<float, Line3<float>, Ellipsoid3<float>>;
    template class FIQuery<float, Line3<float>, Ellipsoid3<float>>;

    template class TIQuery<double, Line3<double>, Ellipsoid3<double>>;
    template class FIQuery<double, Line3<double>, Ellipsoid3<double>>;
}
