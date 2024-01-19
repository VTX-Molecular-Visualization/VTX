#include <catch2/catch_test_macros.hpp>
#include <util/network.hpp>

// network.cpp
TEST_CASE( "Util::Newtork", "[unit]" )
{
	using namespace VTX;

	std::string data;
	VTX::Util::Network::httpRequestGet( "https://vtx.drugdesign.fr/vtx.json", data );

	VTX_INFO( "DATA RECEIVED: {}", data );
}
