#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>

// json.hpp
TEST_CASE( "Util::JSon", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	JSon::Document document = 10;
	nlohmann::json json		= 10;
	CHECK( document.toString() == json.dump() );

	document = { 10 };
	json	 = { 10 };
	CHECK( document.toString() == json.dump() );

	document = { 10, 20, "Zouzou" };
	json	 = { 10, 20, "Zouzou" };
	CHECK( document.toString() == json.dump() );

	document = { { "INT_VALUE", 10 }, { "FLOAT_VALUE", 15.2f }, { "STR_VALUE", "I'm a string" } };
	json	 = { { "INT_VALUE", 10 }, { "FLOAT_VALUE", 15.2f }, { "STR_VALUE", "I'm a string" } };
	CHECK( document.toString() == json.dump() );

	JSon::Document subDocument = { { "MAJOR", 1 }, { "MINOR", 0 }, { "REVISION", { 1234, 'b' } } };
	nlohmann::json subJSon	   = { { "MAJOR", 1 }, { "MINOR", 0 }, { "REVISION", { 1234, 'b' } } };
	CHECK( subDocument.toString() == subJSon.dump() );

	document = { { "VERSION", subDocument } };
	json	 = { { "VERSION", subJSon } };
	CHECK( document.toString() == json.dump() );

	// JSon::write( document );

	// document = JSon::open( "My/Path.json" );

	// Create a document								//OK
	// Write different kind of data in the document		//OK
	// Write the document in a file
	//
	// Open the file
	// Read and check the data
	// Check errors
}
