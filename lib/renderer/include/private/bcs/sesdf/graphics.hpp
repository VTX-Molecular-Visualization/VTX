#ifndef BCS_SESDF_GRAPHICS_HPP
#define BCS_SESDF_GRAPHICS_HPP

#include "bcs/core/type.hpp"
#include "bcs/cuda/math.cuh"

using GLuint = unsigned int;
namespace bcs::sesdf
{
    struct SesdfGraphics
    {
        uint32_t           atomNb = 0;
        HandleSpan<GLuint> atoms;

        // Convex patches data
        uint32_t           convexPatchNb = 0;
        HandleSpan<GLuint> convexPatches;

        uint32_t           sectorNb = 0;
        HandleSpan<GLuint> sectors;

        // Toroidal patches
        uint32_t           segmentPatchNb = 0;
        HandleSpan<GLuint> segmentPatches;

        uint32_t           circlePatchNb = 0;
        HandleSpan<GLuint> circlePatches;

        // Concave patches
        uint32_t           concavePatchNb = 0;
        HandleSpan<GLuint> concavePatchesPosition;
        HandleSpan<GLuint> concavePatchesId;
        HandleSpan<GLuint> concavePatchesNeighbors;

        float    probeRadius         = 0.f;
        uint16_t maxConcaveNeighbors = 0;
    };

    struct SesdfData
    {
        uint32_t atomNb = 0;
        float4 * atoms;

        // Convex patches data
        uint32_t convexPatchNb = 0;
        uint2 *  convexPatches;

        uint32_t sectorNb = 0;
        float4 * sectors;

        // Toroidal patches
        uint32_t segmentPatchNb = 0;
        uint4 *  segmentPatches;

        uint32_t circlePatchNb = 0;
        uint2 *  circlePatches;

        // Concave patches
        uint32_t concavePatchNb = 0;
        float4 * concavePatchesPosition;
        int4 *   concavePatchesId;
        float4 * concavePatchesNeighbors;

        float    probeRadius         = 0.f;
        uint16_t maxConcaveNeighbors = 0;
    };
} // namespace bcs::sesdf

#endif // BCS_SESDF_GRAPHICS_HPP
