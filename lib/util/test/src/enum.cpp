#include <catch2/catch_test_macros.hpp>
#include <util/enum.hpp>

// enum.hpp
enum struct E_EXAMPLE_1
{
	FIRST,
	SECOND
};

enum struct E_EXAMPLE_2
{
	FIRST = 42,
	OTHER,
	SECOND
};

TEST_CASE( "Util::Enum", "[unit]" )
{
	using namespace VTX::Util;

	CHECK( Enum::enumName( E_EXAMPLE_1::FIRST ).compare( "FIRST" ) == 0 );
	CHECK( Enum::enumCast<E_EXAMPLE_1>( "SECOND" ) == E_EXAMPLE_1::SECOND );
	CHECK( Enum::enumToAnother<E_EXAMPLE_1, E_EXAMPLE_2>( E_EXAMPLE_1::SECOND ) == E_EXAMPLE_2::SECOND );
	CHECK( Enum::enumInteger( Enum::enumToAnother<E_EXAMPLE_1, E_EXAMPLE_2>( E_EXAMPLE_1::SECOND ) ) == 44 );
}
