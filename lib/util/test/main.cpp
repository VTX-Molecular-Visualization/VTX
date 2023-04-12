#include <catch2/catch_test_macros.hpp>
#include <util/string.hpp>

TEST_CASE( "Util::String::trimStart", "[string]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trimStart( str );
	REQUIRE( str == "test   " );
}

TEST_CASE( "Util::String::trimEnd", "[string]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trimEnd( str );
	REQUIRE( str == "   test" );
}

TEST_CASE( "Util::String::trim", "[string]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trim( str );
	REQUIRE( str == "test" );
}

TEST_CASE( "Util::String::replaceAll", "[string]" )
{
	std::string str = "a string with characters to replace";
	VTX::Util::String::replaceAll( str, "r", "t" );
	REQUIRE( str == "a stting with chatactets to teplace" );
}

TEST_CASE( "Util::String::floatToStr", "[string]" )
{
	const float f = 3.14159;
	REQUIRE( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	REQUIRE( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	REQUIRE( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );
}

TEST_CASE( "Util::String::strToUint", "[string]" )
{
	const std::string str = "3.14159";
	REQUIRE( VTX::Util::String::strToUint( str ) == 3 );
}
