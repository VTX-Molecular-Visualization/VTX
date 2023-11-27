#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <util/exceptions.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>

// json.hpp
TEST_CASE( "Util::JSon", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	JSon::Document document = 10;
	nlohmann::json json		= 10;
	CHECK( document.getContentAsString() == json.dump() );

	document = { 10 };
	json	 = { 10 };
	CHECK( document.getContentAsString() == json.dump() );

	document = { 10, 20, "Zouzou" };
	json	 = { 10, 20, "Zouzou" };
	CHECK( document.getContentAsString() == json.dump() );

	document = { { "INT_VALUE", 10 }, { "FLOAT_VALUE", 15.2f }, { "STR_VALUE", "I'm a string" } };
	json	 = { { "INT_VALUE", 10 }, { "FLOAT_VALUE", 15.2f }, { "STR_VALUE", "I'm a string" } };
	CHECK( document.getContentAsString() == json.dump() );

	JSon::Document subDocument = { { "MAJOR", 1 }, { "MINOR", 0 }, { "REVISION", { 1234.f, 'b', "Gloubiboulga" } } };
	nlohmann::json subJSon	   = { { "MAJOR", 1 }, { "MINOR", 0 }, { "REVISION", { 1234.f, 'b', "Gloubiboulga" } } };
	CHECK( subDocument.getContentAsString() == subJSon.dump() );

	document = { { "VERSION", subDocument } };
	json	 = { { "VERSION", subJSon } };
	CHECK( document.getContentAsString() == json.dump() );

	if ( !std::filesystem::exists( Filesystem::getExecutableDir() / "data" ) )
		std::filesystem::create_directory( Filesystem::getExecutableDir() / "data" );

	const FilePath jsonPath = Filesystem::getExecutableDir() / "data/json_test.json";
	try
	{
		JSon::IO::write( jsonPath, document );
	}
	catch ( const IOException & exception )
	{
		VTX_ERROR( "Exception when writting JSon : {}", exception.what() );
	}
	REQUIRE( std::filesystem::exists( jsonPath ) );

	try
	{
		document = JSon::IO::open( jsonPath );
	}
	catch ( const IOException & exception )
	{
		VTX_ERROR( "Exception when reading JSon : {}", exception.what() );
	}

	CHECK( document.json().contains( "VERSION" ) );
	CHECK( document.json()[ "VERSION" ].contains( "MAJOR" ) );
	CHECK( document.json()[ "VERSION" ][ "MAJOR" ].get<int>() == 1 );

	const JSon::Object & versionObj = document.json()[ "VERSION" ].getObject();
	REQUIRE( versionObj[ "REVISION" ][ 0 ].get<float>() == 1234.f );
	REQUIRE( versionObj[ "REVISION" ][ 1 ].get<char>() == 'b' );
	REQUIRE( versionObj[ "REVISION" ][ 2 ].get<std::string>() == "Gloubiboulga" );
}
