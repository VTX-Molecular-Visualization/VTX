#include "util/serialization.hpp"
#include "util/app.hpp"
#include <app/core/serialization/deserialization_process.hpp>
#include <app/core/serialization/serialization.hpp>
#include <app/core/serialization/serialization_process.hpp>
#include <app/core/serialization/upgrade_utility.hpp>
#include <app/core/serialization/version.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <filesystem>
#include <string>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>

template<typename T>
T random( const T p_min, const T p_max )
{
	return Catch::Generators::random( p_min, p_max ).get();
}
template<>
VTX::Vec3f random( const VTX::Vec3f p_min, const VTX::Vec3f p_max )
{
	return { random( p_min.x, p_max.x ), random( p_min.y, p_max.y ), random( p_min.z, p_max.z ) };
}

TEST_CASE( "VTX_APP - Serialization", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();

	const Util::Color::Rgba color = { random( 0.f, 1.f ), random( 0.f, 1.f ), random( 0.f, 1.f ), random( 0.f, 1.f ) };

	CHECK( Test::Util::Serialization::checkSerialization( color ) );

	const Util::Math::Range<size_t> range = Util::Math::Range<size_t>( random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( range ) );

	Util::Math::RangeList<int> rangeList = Util::Math::RangeList<int>();
	for ( int i = Catch::Generators::random( 5, 20 ).get(); i >= 0; i-- )
	{
		rangeList.addRange( Util::Math::Range<int>( random( 0, 1000 ), random( 0, 30 ) ) );
	}
	CHECK( Test::Util::Serialization::checkSerialization( rangeList ) );

	const Util::Math::Transform transform = { random( Vec3f( 0.f, 0.f, 0.f ), Vec3f( 100.f, 100.f, 100.f ) ),
											  random( Vec3f( 0.f, 0.f, 0.f ), Vec3f( 100.f, 100.f, 100.f ) ),
											  random( Vec3f( 0.f, 0.f, 0.f ), Vec3f( 100.f, 100.f, 100.f ) ) };

	CHECK( Test::Util::Serialization::checkSerialization<Util::Math::Transform>(
		transform,
		[]( const Util::Math::Transform & p_lhs, const Util::Math::Transform & p_rhs )
		{
			for ( int i = 0; i < 4; i++ )
				for ( int j = 0; j < 4; j++ )
				{
					if ( ( p_lhs.get()[ i ][ j ] - p_rhs.get()[ i ][ j ] ) > EPSILON )
						return false;
				}

			return true;
		}
	) );
}

TEST_CASE( "VTX_APP - Serialization - Custom Obj", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const VTX::Util::JSon::Document jsonDoc = { { "CUSTOM_CLASS", SERIALIZER().serialize( custom ) } };

	REQUIRE( jsonDoc.json().contains( "CUSTOM_CLASS" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ].contains( "COLOR" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "COLOR" ][ "R" ].getFloatingPoint() == COLOR_BLUE.getR() );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "COLOR" ][ "G" ].getFloatingPoint() == COLOR_BLUE.getG() );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "COLOR" ][ "B" ].getFloatingPoint() == COLOR_BLUE.getB() );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "COLOR" ][ "A" ].getFloatingPoint() == COLOR_BLUE.getA() );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ].contains( "STR" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "STR" ].getString() == "strValue" );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ].contains( "INT" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "INT" ].get<int>() == 42 );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ].contains( "VEC" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "VEC" ][ "X" ].get<float>() == 1.f );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "VEC" ][ "Y" ].get<float>() == 2.f );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "VEC" ][ "Z" ].get<float>() == 3.f );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ].contains( "ENUM" ) );
	REQUIRE( jsonDoc.json()[ "CUSTOM_CLASS" ][ "ENUM" ].getString() == "TWO" );
}

TEST_CASE( "VTX_APP - Serialization - Read&Write", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const FilePath jsonPath = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest.json";

	App::Core::Serialization::SerializationProcess serialization { jsonPath, &custom };
	serialization.run();

	REQUIRE( std::filesystem::exists( jsonPath ) );

	CustomClass loadedCustom = CustomClass();
	CHECK( custom != loadedCustom );

	App::Core::Serialization::DeserializationProcess deserialization = { jsonPath, &loadedCustom };
	deserialization.run();

	CHECK( custom == loadedCustom );
}

TEST_CASE( "VTX_APP - Serialization - Upgrade", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	SERIALIZER().registerUpgrade<CustomClass>( { 0, 1, 0 }, &CustomClass::upgrade_0_0_0_to_0_1_0 );
	SERIALIZER().registerUpgrade<CustomClass>( { 1, 0, 0 }, &CustomClass::upgrade_0_1_0_to_1_0_0 );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const FilePath jsonPath_0_1_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_1_0.json";
	CustomClass	   loadedCustom_0_1_0 = CustomClass();

	App::Core::Serialization::DeserializationProcess deserialization = { jsonPath_0_1_0, &loadedCustom_0_1_0 };
	deserialization.run();

	CHECK( custom == loadedCustom_0_1_0 );

	const FilePath jsonPath_0_0_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_0_0.json";
	CustomClass	   loadedCustom_0_0_0 = CustomClass();

	deserialization = { jsonPath_0_0_0, &loadedCustom_0_0_0 };
	deserialization.run();

	CHECK( loadedCustom_0_0_0.color == custom.color );
	CHECK( loadedCustom_0_0_0.enumValue == custom.enumValue );
	CHECK( loadedCustom_0_0_0.strValue == custom.strValue );

	const FilePath jsonPath_1_2_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_1_2_0.json";
	CustomClass	   loadedCustom_1_2_0 = CustomClass();

	try
	{
		deserialization = { jsonPath_0_0_0, &loadedCustom_0_0_0 };
		deserialization.run();
	}
	catch ( const IOException & e )
	{
		VTX_INFO( "Deserialization from more recent version catched !" );
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "More recent file check fail : {}", e.what() );
		CHECK( false );
	}
}

TEST_CASE( "VTX_APP - Serialization - Version", "[unit]" )
{
	using namespace VTX;

	using Version = App::Core::Serialization::Version;

	Version version = { 2, 1, 3 };

	// Operator ==
	CHECK( !( version == Version( 1, 3, 10 ) ) );
	CHECK( !( version == Version( 2, 0, 10 ) ) );
	CHECK( !( version == Version( 2, 1, 2 ) ) );
	CHECK( version == version );
	CHECK( !( version == Version( 2, 1, 8 ) ) );
	CHECK( !( version == Version( 2, 4, 0 ) ) );
	CHECK( !( version == Version( 3, 0, 0 ) ) );

	// Operator !=
	CHECK( version != Version( 1, 3, 10 ) );
	CHECK( version != Version( 2, 0, 10 ) );
	CHECK( version != Version( 2, 1, 2 ) );
	CHECK( !( version != version ) );
	CHECK( version != Version( 2, 1, 8 ) );
	CHECK( version != Version( 2, 4, 0 ) );
	CHECK( version != Version( 3, 0, 0 ) );

	// Operator >
	CHECK( version > Version( 1, 3, 10 ) );
	CHECK( version > Version( 2, 0, 10 ) );
	CHECK( version > Version( 2, 1, 2 ) );
	CHECK( !( version > version ) );
	CHECK( !( version > Version( 2, 1, 8 ) ) );
	CHECK( !( version > Version( 2, 4, 0 ) ) );
	CHECK( !( version > Version( 3, 0, 0 ) ) );

	// Operator >=
	CHECK( version >= Version( 1, 3, 10 ) );
	CHECK( version >= Version( 2, 0, 10 ) );
	CHECK( version >= Version( 2, 1, 2 ) );
	CHECK( version >= version );
	CHECK( !( version >= Version( 2, 1, 8 ) ) );
	CHECK( !( version >= Version( 2, 4, 0 ) ) );
	CHECK( !( version >= Version( 3, 0, 0 ) ) );

	// Operator <
	CHECK( !( version < Version( 1, 3, 10 ) ) );
	CHECK( !( version < Version( 2, 0, 10 ) ) );
	CHECK( !( version < Version( 2, 1, 2 ) ) );
	CHECK( !( version < version ) );
	CHECK( version < Version( 2, 1, 8 ) );
	CHECK( version < Version( 2, 4, 0 ) );
	CHECK( version < Version( 3, 0, 0 ) );

	// Operator <=
	CHECK( !( version <= Version( 1, 3, 10 ) ) );
	CHECK( !( version <= Version( 2, 0, 10 ) ) );
	CHECK( !( version <= Version( 2, 1, 2 ) ) );
	CHECK( version <= version );
	CHECK( version <= Version( 2, 1, 8 ) );
	CHECK( version <= Version( 2, 4, 0 ) );
	CHECK( version <= Version( 3, 0, 0 ) );
}
