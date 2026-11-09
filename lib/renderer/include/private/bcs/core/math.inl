#include "bcs/core/math.hpp"
#include <limits>
#include <stdexcept>

namespace bcs
{
	constexpr std::size_t nextPowerOfTwoValue( const std::size_t baseNumber )
	{
		if ( baseNumber > ( std::size_t { 1 } << ( std::numeric_limits<std::size_t>::digits - 1 ) ) )
		{
			throw std::overflow_error( "Next power of two is not representable" );
		}
		std::size_t i = 1;
		while ( baseNumber > i )
		{
			i <<= 1;
		}
		return i;
	}

	constexpr std::size_t nextPowerOfTwoExponent( std::size_t baseNumber ) noexcept
	{
		uint32_t exponent = 0;
		while ( baseNumber >>= 1 )
		{
			exponent++;
		}
		return exponent;
	}
} // namespace bcs
