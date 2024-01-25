#include <catch2/catch_test_macros.hpp>
#include <string>
#include <util/string.hpp>

// string.hpp
TEST_CASE( "Util::String", "[unit]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trimStart( str );
	CHECK( str == "test   " );

	str = "   test   ";
	VTX::Util::String::trimEnd( str );
	CHECK( str == "   test" );

	str = "   test   ";
	VTX::Util::String::trim( str );
	CHECK( str == "test" );

	str = "a string with characters to replace";
	VTX::Util::String::replaceAll( str, "r", "t" );
	CHECK( str == "a stting with chatactets to teplace" );

	const float f = 3.14159;
	CHECK( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	CHECK( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	CHECK( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );

	str = "3.14159";
	CHECK( VTX::Util::String::strToUint( str ) == 3 );

	str = "123 abcDefghijklmnopqrstuvwxyZ.()+";
	VTX::Util::String::toUpper( str );
	CHECK( str == "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ.()+" );
}
