#include <catch2/catch_test_macros.hpp>
#include <util/network.hpp>

// network.cpp
TEST_CASE( "Util::Newtork", "[unit]" ) { VTX::Util::Network::httpRequest( "https://mmtf.rcsb.org/v1.0/full/4HHB" ); }
