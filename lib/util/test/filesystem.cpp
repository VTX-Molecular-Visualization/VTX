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

	VTX::FilePath filePath3( "data_test/test.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath3 );
	CHECK( filePath3.filename().string() == "test_3.txt" );

	VTX::FilePath filePath4( "data_test/test_2.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath4 );
	CHECK( filePath4.filename().string() == "test_3.txt" );

	VTX::FilePath filePath5( "data_test/test_9.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath5 );
	CHECK( filePath5.filename().string() == "test_9.txt" );

	std::ofstream( "data_test/_2.txt" ).close();
	VTX::FilePath filePath6( "data_test/_2.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath6 );
	CHECK( filePath6.filename().string() == "_2_2.txt" );

	std::filesystem::remove_all( "data_test" );
	CHECK( std::filesystem::exists( "data_test" ) == false );

	const VTX::FilePath noParentPath( "data_test_no_parent.txt" );
	VTX::Util::Filesystem::writeFile( noParentPath, "Hello, world!" );
	CHECK( VTX::Util::Filesystem::readPath( noParentPath ) == "Hello, world!" );
	std::filesystem::remove( noParentPath );
}
