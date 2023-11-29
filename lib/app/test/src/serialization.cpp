#include "util/app.hpp"
#include <app/core/serialization/deserialization_process.hpp>
#include <app/core/serialization/serialization.hpp>
#include <app/core/serialization/serialization_process.hpp>
#include <app/core/serialization/upgrade_utility.hpp>
#include <app/core/serialization/version.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <util/logger.hpp>

namespace VTX::App::Test
{
	class CustomClass
	{
	  public:
		enum class CustomEnum
		{
			ZERO,
			ONE,
			TWO,
			THREE
		};

	  public:
		CustomClass() {}
		CustomClass(
			const VTX::Util::Color::Rgba & p_color,
			const std::string &			   p_strValue,
			const int					   p_intValue,
			const Vec3f &				   p_vector,
			CustomEnum					   p_enum
		) :
			color( p_color ),
			strValue( p_strValue ), intValue( p_intValue ), vector( p_vector ), enumValue( p_enum )
		{
		}

		friend bool operator==( const CustomClass & p_lhs, const CustomClass & p_rhs )
		{
			return p_lhs.color == p_rhs.color && p_lhs.strValue == p_rhs.strValue && p_lhs.intValue == p_rhs.intValue
				   && p_lhs.vector == p_rhs.vector && p_lhs.enumValue == p_rhs.enumValue;
		}
		friend bool operator!=( const CustomClass & p_lhs, const CustomClass & p_rhs )
		{
			return p_lhs.color != p_rhs.color || p_lhs.strValue != p_rhs.strValue || p_lhs.intValue != p_rhs.intValue
				   || p_lhs.vector != p_rhs.vector || p_lhs.enumValue != p_rhs.enumValue;
		}

		VTX::Util::Color::Rgba color	 = COLOR_BLACK;
		std::string			   strValue	 = "Default";
		int					   intValue	 = 10;
		Vec3f				   vector	 = VEC3F_ZERO;
		CustomEnum			   enumValue = CustomEnum::ONE;

		static VTX::Util::JSon::Object serialize( const CustomClass & p_obj )
		{
			return { { "COLOR", SERIALIZER().serialize( p_obj.color ) },
					 { "STR", SERIALIZER().serialize( p_obj.strValue ) },
					 { "INT", SERIALIZER().serialize( p_obj.intValue ) },
					 { "VEC", SERIALIZER().serialize( p_obj.vector ) },
					 { "ENUM", SERIALIZER().serialize( p_obj.enumValue ) } };
		}
		static void deserialize( const VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			p_obj.color		= SERIALIZER().deserializeField( p_jsonObj, "COLOR", COLOR_BLACK );
			p_obj.strValue	= SERIALIZER().deserializeField( p_jsonObj, "STR", std::string( "Default" ) );
			p_obj.intValue	= SERIALIZER().deserializeField( p_jsonObj, "INT", 10 );
			p_obj.vector	= SERIALIZER().deserializeField( p_jsonObj, "VEC", VEC3F_ZERO );
			p_obj.enumValue = SERIALIZER().deserializeField( p_jsonObj, "ENUM", CustomEnum::ONE );
		}

		static void upgrade_0_1_0_to_1_0_0( VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			using namespace VTX::App::Core::Serialization;

			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_COLOR", "COLOR" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_STR", "STR" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_INT", "INT" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_VEC", "VEC" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_ENUM", "ENUM" );

			// Removed parameter
			// "OLD_UNKNOWN_FIELD"
		}
		static void upgrade_0_0_0_to_0_1_0( VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			using namespace VTX::App::Core::Serialization;

			UpgradeUtility::renameField( p_jsonObj, "VERY_OLD_NAME_COLOR", "OLD_NAME_COLOR" );

			// "OLD_NAME_STR" => No changes
			// "OLD_INT" recently added => Nothing to do ; will be filled by default value.

			// Manage type change
			const std::string oldVecStr
				= SERIALIZER().deserializeField( p_jsonObj, "OLD_NAME_VEC", std::string( "<Empty>" ) );
			if ( oldVecStr == "Was a string before" )
			{
				const Vec3f convertedVector = { 1.f, 2.f, 3.f };
				p_jsonObj[ "OLD_NAME_VEC" ] = SERIALIZER().serialize( convertedVector );
			}

			// Rename VERY_OLD_UNKNOWN_FIELD
			UpgradeUtility::renameField( p_jsonObj, "VERY_OLD_UNKNOWN_FIELD", "OLD_UNKNOWN_FIELD" );

			// "OLD_NAME_ENUM" => No changes
		}
	};
} // namespace VTX::App::Test

TEST_CASE( "VTX_APP - Serialization", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<Test::CustomClass>( &Test::CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<Test::CustomClass>( &Test::CustomClass::deserialize );

	const Test::CustomClass custom
		= Test::CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, Test::CustomClass::CustomEnum::TWO );

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

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<Test::CustomClass>( &Test::CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<Test::CustomClass>( &Test::CustomClass::deserialize );

	const Test::CustomClass custom
		= Test::CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, Test::CustomClass::CustomEnum::TWO );

	const FilePath jsonPath = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest.json";

	App::Core::Serialization::SerializationProcess serialization { jsonPath, &custom };
	serialization.run();

	REQUIRE( std::filesystem::exists( jsonPath ) );

	Test::CustomClass loadedCustom = Test::CustomClass();
	CHECK( custom != loadedCustom );

	App::Core::Serialization::DeserializationProcess deserialization = { jsonPath, &loadedCustom };
	deserialization.run();

	CHECK( custom == loadedCustom );
}

TEST_CASE( "VTX_APP - Serialization - Upgrade", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<Test::CustomClass>( &Test::CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<Test::CustomClass>( &Test::CustomClass::deserialize );

	SERIALIZER().registerUpgrade<Test::CustomClass>( { 0, 1, 0 }, &Test::CustomClass::upgrade_0_0_0_to_0_1_0 );
	SERIALIZER().registerUpgrade<Test::CustomClass>( { 1, 0, 0 }, &Test::CustomClass::upgrade_0_1_0_to_1_0_0 );

	const Test::CustomClass custom
		= Test::CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, Test::CustomClass::CustomEnum::TWO );

	const FilePath	  jsonPath_0_1_0 = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_1_0.json";
	Test::CustomClass loadedCustom_0_1_0 = Test::CustomClass();

	App::Core::Serialization::DeserializationProcess deserialization = { jsonPath_0_1_0, &loadedCustom_0_1_0 };
	deserialization.run();

	CHECK( custom == loadedCustom_0_1_0 );

	const FilePath	  jsonPath_0_0_0 = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_0_0.json";
	Test::CustomClass loadedCustom_0_0_0 = Test::CustomClass();

	deserialization = { jsonPath_0_0_0, &loadedCustom_0_0_0 };
	deserialization.run();

	CHECK( loadedCustom_0_0_0.color == custom.color );
	CHECK( loadedCustom_0_0_0.enumValue == custom.enumValue );
	CHECK( loadedCustom_0_0_0.strValue == custom.strValue );

	const FilePath	  jsonPath_1_2_0 = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_1_2_0.json";
	Test::CustomClass loadedCustom_1_2_0 = Test::CustomClass();

	try
	{
		deserialization = { jsonPath_0_0_0, &loadedCustom_0_0_0 };
		deserialization.run();
	}
	catch ( const IOException & e )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
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
