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

namespace VTX
{

} // namespace VTX

TEST_CASE( "Util::Generic::EnumFlag", "[unit]" )
{
	enum class E_FLAG_TEST_ENUM : int
	{
		VTX_ENUM_ENABLE_BITMASK,

		BIT_0 = 1 << 0,
		BIT_1 = 1 << 1,
		BIT_2 = 1 << 2,
		BIT_3 = 1 << 3,
		BIT_4 = 1 << 4,
		BIT_5 = 1 << 5,
		BIT_6 = 1 << 6,
		BIT_7 = 1 << 7,
		BIT_8 = 1 << 8,

		NONE = 0,
		ALL	 = 0xFFFF

	};

	using namespace VTX::Util::Enum;

	E_FLAG_TEST_ENUM testFlag = E_FLAG_TEST_ENUM::NONE;
	testFlag				  = testFlag | E_FLAG_TEST_ENUM::BIT_0;
	CHECK( testFlag == E_FLAG_TEST_ENUM::BIT_0 );

	testFlag = testFlag | E_FLAG_TEST_ENUM::BIT_2;
	CHECK( testFlag == ( E_FLAG_TEST_ENUM::BIT_0 | E_FLAG_TEST_ENUM::BIT_2 ) );
	CHECK( hasBits( testFlag, E_FLAG_TEST_ENUM::BIT_0 ) );
	CHECK( !( testFlag & E_FLAG_TEST_ENUM::BIT_1 ) );
	CHECK( ( testFlag & E_FLAG_TEST_ENUM::BIT_2 ) );

	testFlag &= ( E_FLAG_TEST_ENUM::BIT_0 | E_FLAG_TEST_ENUM::BIT_1 );
	CHECK( hasBits( testFlag, E_FLAG_TEST_ENUM::BIT_0 ) );
	CHECK( ( testFlag & E_FLAG_TEST_ENUM::BIT_0 ) );
	CHECK( !( testFlag & E_FLAG_TEST_ENUM::BIT_2 ) );
}
