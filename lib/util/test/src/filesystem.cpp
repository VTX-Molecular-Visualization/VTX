#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <util/filesystem.hpp>

// filesystem.hpp
TEST_CASE( "Util::Filesystem", "[unit]" )
{
	std::filesystem::create_directory( "data_test" );
	const VTX::FilePath filePath( "data_test/test.txt" );
	std::ofstream		file( filePath );
	file << "Hello, world!";
	file.close();

	REQUIRE( std::filesystem::exists( filePath ) );
	CHECK( VTX::Util::Filesystem::readPath( filePath ) == "Hello, world!" );

	VTX::FilePath filePath2( "data_test/test.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath2 );

	REQUIRE( filePath2.filename().string() == "test_2.txt" );
	CHECK( filePath2.string() == VTX::FilePath( "data_test" ) / "test_2.txt" );

	std::ofstream file2( filePath2 );
	file2 << "Hello, world!";
	file2.close();

	VTX::Util::Filesystem::removeAll( "data_test/sdqsdqsd" );
	CHECK( std::filesystem::exists( "data_test" ) == false );
}
