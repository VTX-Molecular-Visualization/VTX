#ifndef BCS_CORE_MATH_HPP
#define BCS_CORE_MATH_HPP

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace bcs
{
    constexpr float Pif    = 3.141592654f;
    constexpr float TwoPif = 6.283185307f;

    constexpr std::size_t nextPowerOfTwoValue( const std::size_t baseNumber ) noexcept;
    constexpr std::size_t nextPowerOfTwoExponent( std::size_t baseNumber ) noexcept;

    using Vec4f = glm::vec4;
    using Vec3f = glm::vec3;

    struct Aabb
    {
        Vec3f min;
        Vec3f max;
    };

} // namespace bcs

#include "bcs/core/math.inl"

#endif // BCS_CORE_MATH_HPP
