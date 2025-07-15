#ifndef BCS_SSESDF_GRAPHICS_HPP
#define BCS_SSESDF_GRAPHICS_HPP

#include "bcs/core/type.hpp"
#include "bcs/cuda/math.cuh"

using GLuint = unsigned int;
namespace bcs::ssesdf
{
    struct SsesdfGraphics
    {
        uint32_t           atomNb = 0;
        HandleSpan<GLuint> atoms;

        // Toroidal patches
        uint32_t           circlePatchNb = 0;
        HandleSpan<GLuint> circlePosition;
        HandleSpan<GLuint> circleAxis;
        HandleSpan<GLuint> circleVs;

        // Concave patches
        uint32_t           concavePatchNb = 0;
        HandleSpan<GLuint> concavePatchesPosition;
        HandleSpan<GLuint> concavePatchesId;
        HandleSpan<GLuint> concavePatchesNeighbors;

        float    probeRadius         = 0.f;
        uint16_t maxConcaveNeighbors = 0;
    };

    struct SsesdfData
    {
        uint32_t atomNb = 0;
        float4 * atoms;

        // Toroidal patches
        uint32_t circlePatchNb = 0;
        float4 * circlePosition;
        float4 * circleAxis;
        float4 * circleVs;

        // Concave patches
        uint32_t concavePatchNb = 0;
        float4 * concavePatchesPosition;
        int4 *   concavePatchesId;
        float4 * concavePatchesNeighbors;

        float    probeRadius         = 0.f;
        uint16_t maxConcaveNeighbors = 0;
    };
} // namespace bcs::ssesdf

#endif // BCS_SSESDF_GRAPHICS_HPP
