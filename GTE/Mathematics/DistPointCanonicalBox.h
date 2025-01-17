// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 6.0.2022.01.06

#pragma once

#include <GTE/Mathematics/DCPQuery.h>
#include <GTE/Mathematics/CanonicalBox.h>

// Compute the distance from a point to a solid canonical box in nD.
// 
// The canonical box has center at the origin and is aligned with the
// coordinate axes. The extents are E = (e[0],e[1],...,e[n-1]). A box
// point is Y = (y[0],y[1],...,y[n-1]) with |y[i]| <= e[i] for all i.
// 
// The input point P is stored in closest[0]. The closest point on the box
// is stored in closest[1]. When there are infinitely many choices for the
// pair of closest points, only one of them is returned.

namespace gte
{
    template <int32_t N, typename T>
    class DCPQuery<T, Vector<N, T>, CanonicalBox<N, T>>
    {
    public:
        struct Result
        {
            Result()
                :
                distance(static_cast<T>(0)),
                sqrDistance(static_cast<T>(0)),
                closest{ Vector<N, T>::Zero(), Vector<N, T>::Zero() }
            {
            }

            T distance, sqrDistance;
            std::array<Vector<N, T>, 2> closest;
        };

        Result operator()(Vector<N, T> const& point, CanonicalBox<N, T> const& box)
        {
            Result result{};

            result.closest[0] = point;
            result.closest[1] = point;
            result.sqrDistance = static_cast<T>(0);
            for (int32_t i = 0; i < N; ++i)
            {
                if (point[i] < -box.extent[i])
                {
                    T delta = result.closest[1][i] + box.extent[i];
                    result.sqrDistance += delta * delta;
                    result.closest[1][i] = -box.extent[i];
                }
                else if (point[i] > box.extent[i])
                {
                    T delta = result.closest[1][i] - box.extent[i];
                    result.sqrDistance += delta * delta;
                    result.closest[1][i] = box.extent[i];
                }
            }
            result.distance = std::sqrt(result.sqrDistance);

            return result;
        }
    };

    // Template aliases for convenience.
    template <int32_t N, typename T>
    using DCPPointCanonicalBox = DCPQuery<T, Vector<N, T>, CanonicalBox<N, T>>;

    template <typename T>
    using DCPPoint2CanonicalBox2 = DCPPointCanonicalBox<2, T>;

    template <typename T>
    using DCPPoint3CanonicalBox3 = DCPPointCanonicalBox<3, T>;
}
