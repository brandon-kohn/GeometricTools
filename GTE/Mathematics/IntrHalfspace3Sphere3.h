// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 6.0.2022.01.06

#pragma once

#include <GTE/Mathematics/TIQuery.h>
#include <GTE/Mathematics/Halfspace.h>
#include <GTE/Mathematics/Hypersphere.h>

// Queries for intersection of objects with halfspaces.  These are useful for
// containment testing, object culling, and clipping.

namespace gte
{
    template <typename T>
    class TIQuery<T, Halfspace3<T>, Sphere3<T>>
    {
    public:
        struct Result
        {
            Result()
                :
                intersect(false)
            {
            }

            bool intersect;
        };

        Result operator()(Halfspace3<T> const& halfspace, Sphere3<T> const& sphere)
        {
            Result result{};

            // Project the sphere center onto the normal line.  The plane of
            // the halfspace occurs at the origin (zero) of the normal line.
            T center = Dot(halfspace.normal, sphere.center) - halfspace.constant;

            // The sphere and halfspace intersect when the projection interval
            // maximum is nonnegative.
            result.intersect = (center + sphere.radius >= (T)0);
            return result;
        }
    };
}
