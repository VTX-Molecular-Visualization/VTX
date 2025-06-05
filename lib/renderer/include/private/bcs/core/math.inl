#include "bcs/core/math.hpp"

namespace bcs
{
    constexpr std::size_t nextPowerOfTwoValue( const std::size_t baseNumber ) noexcept
    {
        std::size_t i = 1;
        while ( baseNumber > i )
            i <<= 1;
        return i;
    }

    constexpr std::size_t nextPowerOfTwoExponent( std::size_t baseNumber ) noexcept
    {
        uint32_t exponent = 0;
        while ( baseNumber >>= 1 )
            exponent++;
        return exponent;
    }
} // namespace bcs
