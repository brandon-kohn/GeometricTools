// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 6.0.2022.03.13

#pragma once

#include <GTE/Mathematics/Logger.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>

// UniqueVerticesTriangles is a helper class that provides support for several
// mesh generation and mesh reduction operations. The vertices have type
// VertexType, which must have a less-than comparison predicate because
// duplicate vertices are eliminated in the operations.
//
//   1. Generate an indexed triangle representation from an array of
//      triples of VertexType. Each triple represents the vertices of
//      a triangle. Presumably, the triangles share vertices. The output
//      is an array of unique VertexType objects (a vertex pool) and an
//      array of triples of indices into the pool, each triple
//      representing a triangle.
//
//   2. Remove duplicate vertices from a vertex pool used by an indexed
//      triangle representation. A new vertex pool of unique vertices is
//      generated and the indexed triangles are modified to be indices into
//      this vertex pool.
//
//   3. Remove unused vertices from a vertex pool used by an indexed triangle
//      representation. A new vertex pool of unique vertices is generated and
//      the indexed triangles are modified to be indices into the new vertex
//      pool.
//
//   4. Remove duplicate and unused vertices from a vertex pool, a combination
//      of the operations in #2 and #3.

// Uncomment this preprocessor symbol to validate the preconditions of the
// inputs of the class member functions.
#define GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES

namespace gte
{
    template <typename VertexType>
    class // [[deprecated("Use UniqueVerticesSimplices<VertexType,IndexType,3> instead.")]]
        UniqueVerticesTriangles
    {
    public:
        // The class has no state, so the constructors, destructors, copy
        // semantics and move semantics are all the defaults generated by the
        // compiler.

        // See #1 in the comments at the beginning of this file. The
        // preconditions are
        //   1. inVertices.size() is a positive multiple of 3
        // The postconditions are
        //   1. outVertices has unique vertices
        //   2. outIndices.size() = inVertices.size()
        //   3. 0 <= outIndices[i] < outVertices.size()
        void GenerateIndexedTriangles(
            std::vector<VertexType> const& inVertices,
            std::vector<VertexType>& outVertices,
            std::vector<int32_t>& outIndices)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0 && inVertices.size() % 3 == 0,
                "Invalid number of vertices.");
#endif
            outIndices.resize(inVertices.size());
            RemoveDuplicates(inVertices, outVertices, outIndices.data());
        }

        // See #1 in the comments at the beginning of this file. The
        // preconditions are
        //   1. inVertices.size() is a positive multiple of 3
        // The postconditions are
        //   1. outVertices has unique vertices
        //   2. outTriangles.size() = inVertices.size() / 3
        //   3. 0 <= outTriangles[i][j] < outVertices.size()
        void GenerateIndexedTriangles(
            std::vector<VertexType> const& inVertices,
            std::vector<VertexType>& outVertices,
            std::vector<std::array<int32_t, 3>>& outTriangles)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0 && inVertices.size() % 3 == 0,
                "Invalid number of vertices.");
#endif
            outTriangles.resize(inVertices.size() / 3);
            int32_t* outIndices = reinterpret_cast<int32_t*>(outTriangles.data());
            RemoveDuplicates(inVertices, outVertices, outIndices);
        }

        // See #2 in the comments at the beginning of the file. The
        // preconditions are
        //   1. inVertices.size() is positive
        //   2. inIndices.size() is a positive multiple of 3
        //   3. 0 <= inIndices[i] < inVertices.size()
        // The postconditions are
        //   1. outVertices has unique vertices
        //   2. outIndices.size() = inIndices.size()
        //   3. 0 <= outIndices[i] < outVertices.size()
        void RemoveDuplicateVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<int32_t> const& inIndices,
            std::vector<VertexType>& outVertices,
            std::vector<int32_t>& outIndices)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0, "Invalid number of vertices.");
            LogAssert(inIndices.size() > 0 && inIndices.size() % 3 == 0,
                "Invalid number of indices.");
            int32_t const numVertices = static_cast<int32_t>(inVertices.size());
            for (auto index : inIndices)
            {
                LogAssert(0 <= index && index < numVertices, "Invalid index.");
            }
#endif

            std::vector<int32_t> inToOutMapping(inVertices.size());
            RemoveDuplicates(inVertices, outVertices, inToOutMapping.data());

            outIndices.resize(inIndices.size());
            for (size_t i = 0; i < inIndices.size(); ++i)
            {
                outIndices[i] = inToOutMapping[inIndices[i]];
            }
        }

        // See #2 in the comments at the beginning of the file. The
        // preconditions are
        //   1. inVertices.size() is positive
        //   2. inTriangles.size() is positive
        //   3. 0 <= inTriangles[i][j] < inVertices.size()
        // The postconditions are
        //   1. outVertices has unique vertices
        //   2. outTriangles.size() = inTriangles.size()
        //   3. 0 <= outTriangles[i][j] < outVertices.size()
        void RemoveDuplicateVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<std::array<int32_t, 3>> const& inTriangles,
            std::vector<VertexType>& outVertices,
            std::vector<std::array<int32_t, 3>>& outTriangles)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0, "Invalid number of vertices.");
            LogAssert(inTriangles.size() > 0, "Invalid number of triangles.");
            int32_t const numVertices = static_cast<int32_t>(inVertices.size());
            for (auto const& triangle : inTriangles)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    LogAssert(0 <= triangle[j] && triangle[j] < numVertices,
                        "Invalid index.");
                }
            }
#endif

            std::vector<int32_t> inToOutMapping(inVertices.size());
            RemoveDuplicates(inVertices, outVertices, inToOutMapping.data());

            size_t const numTriangles = inTriangles.size();
            outTriangles.resize(numTriangles);
            for (size_t t = 0; t < numTriangles; ++t)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    outTriangles[t][j] = inToOutMapping[inTriangles[t][j]];
                }
            }
        }

        // See #3 in the comments at the beginning of the file. The
        // preconditions are
        //   1. inVertices.size() is positive
        //   2. inIndices.size() is a positive multiple of 3
        //   3. 0 <= inIndices[i] < inVertices.size()
        // The postconditions are
        //   1. outVertices.size() is positive
        //   2. outIndices.size() = inIndices.size()
        //   3. 0 <= outIndices[i] < outVertices.size()
        //   4. each outVertices[j] occurs at least once in outIndices
        void RemoveUnusedVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<int32_t> const& inIndices,
            std::vector<VertexType>& outVertices,
            std::vector<int32_t>& outIndices)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0, "Invalid number of vertices.");
            LogAssert(inIndices.size() > 0 && inIndices.size() % 3 == 0,
                "Invalid number of indices.");
            int32_t const numVertices = static_cast<int32_t>(inVertices.size());
            for (auto index : inIndices)
            {
                LogAssert(0 <= index && index < numVertices, "Invalid index.");
            }
#endif
            outIndices.resize(inIndices.size());
            RemoveUnused(inVertices, inIndices.size(), inIndices.data(),
                outVertices, outIndices.data());
        }

        // See #3 in the comments at the beginning of the file. The
        // preconditions are
        //   1. inVertices.size() is positive
        //   2. inTriangles.size() is positive
        //   3. 0 <= outTriangles[i][j] < inVertices.size()
        // The postconditions are
        //   1. outVertices.size() is positive
        //   2. outTriangles.size() = inTriangles.size()
        //   3. 0 <= outTriangles[i][j] < outVertices.size()
        //   4. each outVertices[j] occurs at least once in outTriangles
        void RemoveUnusedVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<std::array<int32_t, 3>> const& inTriangles,
            std::vector<VertexType> & outVertices,
            std::vector<std::array<int32_t, 3>> & outTriangles)
        {
#if defined(GTL_VALIDATE_UNIQUE_VERTICES_TRIANGLES)
            LogAssert(inVertices.size() > 0, "Invalid number of vertices.");
            LogAssert(inTriangles.size() > 0, "Invalid number of triangles.");
            int32_t const numVertices = static_cast<int32_t>(inVertices.size());
            for (auto const& triangle : inTriangles)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    LogAssert(0 <= triangle[j] && triangle[j] < numVertices,
                        "Invalid index.");
                }
            }
#endif
            outTriangles.resize(inTriangles.size());
            size_t const numInIndices = 3 * inTriangles.size();
            int32_t const* inIndices = reinterpret_cast<int32_t const*>(inTriangles.data());
            int32_t* outIndices = reinterpret_cast<int32_t*>(outTriangles.data());
            RemoveUnused(inVertices, numInIndices, inIndices, outVertices, outIndices);
        }

        // See #4 and the preconditions for RemoveDuplicateVertices and for
        // RemoveUnusedVertices.
        void RemoveDuplicateAndUnusedVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<int32_t> const& inIndices,
            std::vector<VertexType>& outVertices,
            std::vector<int32_t>& outIndices)
        {
            std::vector<VertexType> tempVertices;
            std::vector<int32_t> tempIndices;
            RemoveDuplicateVertices(inVertices, inIndices, tempVertices, tempIndices);
            RemoveUnusedVertices(tempVertices, tempIndices, outVertices, outIndices);
        }

        // See #4 and the preconditions for RemoveDuplicateVertices and for
        // RemoveUnusedVertices.
        void RemoveDuplicateAndUnusedVertices(
            std::vector<VertexType> const& inVertices,
            std::vector<std::array<int32_t, 3>> const& inTriangles,
            std::vector<VertexType>& outVertices,
            std::vector<std::array<int32_t, 3>>& outTriangles)
        {
            std::vector<VertexType> tempVertices;
            std::vector<std::array<int32_t, 3>> tempTriangles;
            RemoveDuplicateVertices(inVertices, inTriangles, tempVertices, tempTriangles);
            RemoveUnusedVertices(tempVertices, tempTriangles, outVertices, outTriangles);
        }

    private:
        void RemoveDuplicates(
            std::vector<VertexType> const& inVertices,
            std::vector<VertexType>& outVertices,
            int32_t* inToOutMapping)
        {
            // Construct the unique vertices.
            size_t const numInVertices = inVertices.size();
            size_t numOutVertices = 0;
            std::map<VertexType, int32_t> vmap;
            for (size_t i = 0; i < numInVertices; ++i, ++inToOutMapping)
            {
                auto const iter = vmap.find(inVertices[i]);
                if (iter != vmap.end())
                {
                    // The vertex is a duplicate of one inserted earlier into
                    // the map. Its index i will be modified to that of the
                    // first-found vertex.
                    *inToOutMapping = iter->second;
                }
                else
                {
                    // The vertex occurs for the first time.
                    vmap.insert(std::make_pair(inVertices[i],
                        static_cast<int32_t>(numOutVertices)));
                    *inToOutMapping = static_cast<int32_t>(numOutVertices);
                    ++numOutVertices;
                }
            }

            // Pack the unique vertices into an array.
            outVertices.resize(numOutVertices);
            for (auto const& element : vmap)
            {
                outVertices[element.second] = element.first;
            }
        }

        void RemoveUnused(
            std::vector<VertexType> const& inVertices,
            size_t const numInIndices,
            int32_t const* inIndices,
            std::vector<VertexType>& outVertices,
            int32_t* outIndices)
        {
            std::set<int32_t> usedIndices;
            for (size_t i = 0; i < numInIndices; ++i)
            {
                usedIndices.insert(inIndices[i]);
            }

            // Locate the used vertices and pack them into an array.
            outVertices.resize(usedIndices.size());
            size_t numOutVertices = 0;
            std::map<int32_t, int32_t> vmap;
            for (auto oldIndex : usedIndices)
            {
                outVertices[numOutVertices] = inVertices[oldIndex];
                vmap.insert(std::make_pair(oldIndex, static_cast<int32_t>(numOutVertices)));
                ++numOutVertices;
            }

            // Reassign the old indices to the new indices.
            for (size_t i = 0; i < numInIndices; ++i)
            {
                outIndices[i] = vmap.find(inIndices[i])->second;
            }
        }
    };
}
