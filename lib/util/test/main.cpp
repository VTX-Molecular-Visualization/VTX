#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <util/filesystem.hpp>
#include <util/generic/base_static_singleton.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[logger]" )
{
	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
}

// string.hpp
TEST_CASE( "Util::String", "[string]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trimStart( str );
	REQUIRE( str == "test   " );

	str = "   test   ";
	VTX::Util::String::trimEnd( str );
	REQUIRE( str == "   test" );

	str = "   test   ";
	VTX::Util::String::trim( str );
	REQUIRE( str == "test" );

	str = "a string with characters to replace";
	VTX::Util::String::replaceAll( str, "r", "t" );
	REQUIRE( str == "a stting with chatactets to teplace" );

	const float f = 3.14159;
	REQUIRE( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	REQUIRE( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	REQUIRE( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );

	str = "3.14159";
	REQUIRE( VTX::Util::String::strToUint( str ) == 3 );
}

// filesystem.hpp
TEST_CASE( "Util::Filesystem", "[filesystem]" )
{
	std::filesystem::create_directory( "data_test" );
	const VTX::FilePath filePath( "data_test/test.txt" );
	std::ofstream		file( filePath );
	file << "Hello, world!";
	file.close();

	REQUIRE( std::filesystem::exists( filePath ) );
	REQUIRE( VTX::Util::Filesystem::readPath( filePath ) == "Hello, world!" );

	VTX::FilePath filePath2( "data_test/test.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath2 );

	REQUIRE( filePath2.filename().string() == "test_2.txt" );
	REQUIRE( filePath2.string() == VTX::FilePath( "data_test" ) / "test_2.txt" );

	std::ofstream file2( filePath2 );
	file2 << "Hello, world!";
	file2.close();

	VTX::Util::Filesystem::removeAll( "data_test/sdqsdqsd" );
	REQUIRE( std::filesystem::exists( "data_test" ) == false );
}

// base_static_singleton.hpp
TEST_CASE( "Util::BaseStaticSingleton", "[generic]" )
{
	class SingletonTest : public VTX::Util::Generic::BaseStaticSingleton<SingletonTest>
	{
	  public:
		SingletonTest( const StructPrivacyToken & ) {}
		SingletonTest( std::initializer_list<int> ) = delete;
	};

	const SingletonTest & instance = VTX::Util::Generic::BaseStaticSingleton<SingletonTest>::get();

	// Forbidden.
	// const SingletonTest consDefault;
	// const SingletonTest consCopy( instance );
	// const SingletonTest consMove( std::move( instance ) );
	// const SingletonTest consInit( {} );
}
