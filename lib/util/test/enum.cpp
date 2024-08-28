#include <catch2/catch_test_macros.hpp>
#include <util/enum.hpp>
#include <util/enum_flag.hpp>

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
	enum class E_FLAG_TEST_ENUM : int
	{
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

	VTX_FLAG( E_FLAG_TEST_FLAG, E_FLAG_TEST_ENUM );
} // namespace VTX

TEST_CASE( "Util::Generic::EnumFlag", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	E_FLAG_TEST_FLAG testFlag = E_FLAG_TEST_ENUM::NONE;
	testFlag				  = testFlag | E_FLAG_TEST_ENUM::BIT_0;
	CHECK( testFlag == E_FLAG_TEST_ENUM::BIT_0 );

	testFlag = testFlag | E_FLAG_TEST_FLAG::ENUM::BIT_2;
	CHECK( testFlag == ( E_FLAG_TEST_ENUM::BIT_0 | E_FLAG_TEST_ENUM::BIT_2 ) );
	CHECK( testFlag & E_FLAG_TEST_ENUM::BIT_0 );
	CHECK( !( testFlag & E_FLAG_TEST_ENUM::BIT_1 ) );
	CHECK( testFlag & E_FLAG_TEST_ENUM::BIT_2 );

	testFlag &= ( E_FLAG_TEST_ENUM::BIT_0 | E_FLAG_TEST_ENUM::BIT_1 );
	CHECK( testFlag == E_FLAG_TEST_ENUM::BIT_0 );
	CHECK( testFlag & E_FLAG_TEST_ENUM::BIT_0 );
	CHECK( !( testFlag & E_FLAG_TEST_ENUM::BIT_2 ) );
}
