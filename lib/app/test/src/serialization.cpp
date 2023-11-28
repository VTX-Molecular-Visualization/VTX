#include "util/app.hpp"
#include <app/core/serialization/serialization.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <util/color/rgba.hpp>
#include <util/json/json.hpp>
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
			SERIALIZER().deserialize( p_jsonObj[ "COLOR" ], p_obj.color );
			SERIALIZER().deserialize( p_jsonObj[ "STR" ], p_obj.strValue );
			SERIALIZER().deserialize( p_jsonObj[ "INT" ], p_obj.intValue );
			SERIALIZER().deserialize( p_jsonObj[ "VEC" ], p_obj.vector );
			SERIALIZER().deserialize( p_jsonObj[ "ENUM" ], p_obj.enumValue );
		}
	};
} // namespace VTX::App::Test

TEST_CASE( "VTX_APP - Serialization", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	VTX_INFO( "TEST VTX_APP - Serialization" );

	Test::Util::App::initApp();

	SERIALIZER().registerSerializationFunction<Test::CustomClass>( &Test::CustomClass::serialize );
	SERIALIZER().registerDeserializationFunction<Test::CustomClass>( &Test::CustomClass::deserialize );

	const Test::CustomClass custom
		= Test::CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, Test::CustomClass::CustomEnum::TWO );

	const VTX::Util::JSon::Document jsonDoc = { { "CUSTOM_CLASS", SERIALIZER().serialize( custom ) } };

	const FilePath jsonPath = Util::Filesystem::getExecutableDir() / "data/jsonTest.json";

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

	SERIALIZER().writeFile( jsonPath, jsonDoc );
	REQUIRE( std::filesystem::exists( jsonPath ) );

	const VTX::Util::JSon::Document loadedJsonDoc = SERIALIZER().readFile( jsonPath );

	Test::CustomClass loadedCustom = Test::CustomClass();
	CHECK( custom != loadedCustom );

	SERIALIZER().deserialize( jsonDoc.json()[ "CUSTOM_CLASS" ], loadedCustom );
	CHECK( custom == loadedCustom );
}
