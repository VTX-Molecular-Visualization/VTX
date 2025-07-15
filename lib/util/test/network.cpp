#include <catch2/catch_test_macros.hpp>
#include <util/network.hpp>

// network.cpp
TEST_CASE( "Util::Network", "[unit]" )
{
	using namespace VTX;
	using namespace Util::Network;

	std::string text;
	httpRequestGet( "https://vtx.drugdesign.fr/vtx.json", text );
	CHECK( text.size() == 96 );
}
