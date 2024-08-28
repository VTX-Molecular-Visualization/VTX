#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <util/exceptions.hpp>
#include <util/json/io.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>

// json.hpp
TEST_CASE( "Util::JSon", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	JSon::Document document = JSon::BasicJSon( 10 );
	nlohmann::json json		= 10;
	CHECK( document.getContentAsString() == json.dump() );

	document = { 10 };
	json	 = { 10 };
	CHECK( document.getContentAsString() == json.dump() );

	document = { 10, true, "Zouzou" };
	json	 = { 10, true, "Zouzou" };
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

TEST_CASE( "Util::JSon - Object Field", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	const JSon::Document document = { { "BOOL_TRUE", true },
									  { "BOOL_FALSE", false },
									  { "INT", 10 },
									  { "CHAR", 'u' },
									  { "SIZE_T", size_t( 10 ) },
									  { "LONG", long( 10 ) },
									  { "UINT", static_cast<unsigned int>( 10 ) },
									  { "FLOAT", 10.f },
									  { "DOUBLE", 10. },
									  { "STRING", std::string( "str" ) },
									  { "ARRAY", { 10, 20, 30 } },
									  { "OBJECT", { { "FIELD_A", 10 }, { "FIELD_B", "str" } } } };

	CHECK( document.json()[ "BOOL_TRUE" ].get<bool>() == true );
	CHECK( document.json()[ "BOOL_TRUE" ].getBool() == true );
	CHECK( document.json()[ "BOOL_FALSE" ].get<bool>() == false );
	CHECK( document.json()[ "BOOL_FALSE" ].getBool() == false );
	CHECK( document.json()[ "INT" ].get<int>() == 10 );
	CHECK( document.json()[ "CHAR" ].get<char>() == 'u' );
	CHECK( document.json()[ "SIZE_T" ].get<size_t>() == 10 );
	CHECK( document.json()[ "LONG" ].get<long>() == 10 );
	CHECK( document.json()[ "UINT" ].get<unsigned int>() == 10 );
	CHECK( document.json()[ "FLOAT" ].get<float>() == 10.f );
	CHECK( document.json()[ "DOUBLE" ].get<double>() == 10. );
	CHECK( document.json()[ "STRING" ].get<std::string>() == "str" );
	CHECK( document.json()[ "ARRAY" ][ 0 ].get<int>() == 10 );
	CHECK( document.json()[ "ARRAY" ][ 1 ].get<int>() == 20 );
	CHECK( document.json()[ "ARRAY" ][ 2 ].get<int>() == 30 );
	CHECK( document.json()[ "OBJECT" ][ "FIELD_A" ].get<int>() == 10 );
	CHECK( document.json()[ "OBJECT" ][ "FIELD_B" ].get<std::string>() == "str" );
}
