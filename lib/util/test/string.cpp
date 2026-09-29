#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <util/string.hpp>

// string.hpp
TEST_CASE( "Util::String", "[unit]" )
{
	std::string str = "   test   ";
	str				= VTX::Util::String::trimStart( str );
	CHECK( str == "test   " );

	str = "   test   ";
	str = VTX::Util::String::trimEnd( str );
	CHECK( str == "   test" );

	str = "   test   ";
	str = VTX::Util::String::trim( str );
	CHECK( str == "test" );

	str = "a string with characters to replace";
	str = VTX::Util::String::replaceAll( str, "r", "t" );
	CHECK( str == "a stting with chatactets to teplace" );
	const std::string strToReplace = "a string";
	CHECK( VTX::Util::String::replaceAll( strToReplace, "r", "t" ) == "a stting" );

	const float f = 3.14159;
	CHECK( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	CHECK( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	CHECK( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );

	CHECK( VTX::Util::String::strToNumber<VTX::uint>( "3" ) == 3u );
	CHECK( VTX::Util::String::strToNumber<int>( "-3" ) == -3 );
	CHECK_THROWS_AS( VTX::Util::String::strToNumber<VTX::uint>( "3.14159" ), std::invalid_argument );
	CHECK_THROWS_AS( VTX::Util::String::strToNumber<VTX::uint>( "" ), std::invalid_argument );
	CHECK_THROWS_AS( VTX::Util::String::strToNumber<VTX::uint>( "-1" ), std::invalid_argument );
	CHECK_THROWS_AS(
		VTX::Util::String::strToNumber<VTX::uint>( std::to_string( std::numeric_limits<VTX::uint>::max() ) + "0" ),
		std::invalid_argument
	);

	str = "123 abcDefghijklmnopqrstuvwxyZ.()+";
	str = VTX::Util::String::toUpper( str );
	CHECK( str == "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ.()+" );
}
