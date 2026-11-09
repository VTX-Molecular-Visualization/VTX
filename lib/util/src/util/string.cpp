#include "util/string.hpp"
#include "util/fmt/duration.hpp"
#include "util/fmt/mem_size.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <iomanip>
#include <sstream>

namespace VTX::Util::String
{

	std::string floatToStr( const float p_value, const int p_nbDecimals )
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( p_nbDecimals ) << p_value;
		return strStream.str();
	}

	std::string toUpper( const std::string & p_str )
	{
		std::string result = p_str;

		std::ranges::transform(
			result,
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::toupper( p_char ) ); }
		);

		return result;
	}

	std::string toLower( const std::string & p_str )
	{
		std::string result = p_str;

		std::ranges::transform(
			result,
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::tolower( p_char ) ); }
		);

		return result;
	}

	std::string memSizeToStr( const size_t p_size, const bool p_isBase10 )
	{ return fmt::format( "{}", FormattedMemSize { p_size, p_isBase10 } ); }

	std::string durationToStr( const float p_durationInMs )
	{ return fmt::format( "{}", FormattedDuration { p_durationInMs } ); }

} // namespace VTX::Util::String
