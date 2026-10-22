#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <util/string.hpp>

static_assert(
	[]
	{
		using namespace VTX::Util::String;
		return trim( " \talpha \n" ) == "alpha" && trim( " \t" ).empty() && trimStart( "  alpha " ) == "alpha "
			   && trimEnd( " alpha  " ) == " alpha"
			   && split( "a,,b,", ',' ) == std::vector<std::string> { "a", "", "b", "" }
			   && replaceFirst( "aaa", "a", "b" ) == "baa" && replaceAll( "aaaa", "aa", "b" ) == "bb"
			   && replaceAll( "aa", "a", "aa" ) == "aaaa" && replaceAll( "aaa", "a", "a" ) == "aaa"
			   && replaceAll( "aaa", "a", "" ).empty() && replaceAll( "abc", "", "x" ) == "abc";
	}()
);

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

	CHECK( VTX::Util::String::replaceFirst( "a[token]z", "[token]", "id" ) == "aidz" );
	CHECK( VTX::Util::String::replaceFirst( "[token][token]", "[token]", "id" ) == "id[token]" );
	CHECK( VTX::Util::String::replaceFirst( "abc", "b", "long" ) == "alongc" );
	CHECK( VTX::Util::String::replaceFirst( "abc", "b", "" ) == "ac" );
	CHECK_THROWS_AS( VTX::Util::String::replaceFirst( "abc", "missing", "id" ), std::out_of_range );

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
