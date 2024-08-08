#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <util/hashing.hpp>

// string.hpp
TEST_CASE( "Util::Hashing", "[unit]" )
{
	using namespace VTX;
	using namespace Util;

	// Hash Class
	{
		const int value1 = 1;
		const int value2 = 2;
		const int value3 = value1;

		const Hash hashedValue1 = hash( value1 );
		const Hash hashedValue2 = hash( value2 );
		const Hash hashedValue3 = hash( value3 );

		CHECK( hashedValue1 != hashedValue2 );
		CHECK( hashedValue2 != hashedValue3 );
		CHECK( hashedValue1 == hashedValue3 );
	}

	{
		const std::unique_ptr<float> value1 = std::make_unique<float>( 1.f );
		const std::unique_ptr<float> value2 = std::make_unique<float>( 2.f );
		const float *				 value3 = value1.get();

		const Hash hashedValue1 = hash( value1.get() );
		const Hash hashedValue2 = hash( value2.get() );
		const Hash hashedValue3 = hash( value3 );

		CHECK( hashedValue1 != hashedValue2 );
		CHECK( hashedValue2 != hashedValue3 );
		CHECK( hashedValue1 == hashedValue3 );
	}

	{
		const std::string value1 = "Zouzou";
		const std::string value2 = "Zaza";
		const std::string value3 = value1;

		const Hash hashedStr1 = hash( value1 );
		const Hash hashedStr2 = hash( value2 );
		const Hash hashedStr3 = hash( value3 );

		CHECK( hashedStr1 != hashedStr2 );
		CHECK( hashedStr2 != hashedStr3 );
		CHECK( hashedStr1 == hashedStr3 );
	}

	{
		const char * value1 = "Zouzou";
		const char * value2 = "Zaza";
		const char * value3 = value1;

		const Hash hashedStr1 = hash( value1 );
		const Hash hashedStr2 = hash( value2 );
		const Hash hashedStr3 = hash( value3 );

		CHECK( hashedStr1 != hashedStr2 );
		CHECK( hashedStr2 != hashedStr3 );
		CHECK( hashedStr1 == hashedStr3 );
	}
}
