#include "util/app.hpp"
#include <app/core/settings/settings_system.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/color/rgba.hpp>

TEST_CASE( "VTX_APP - Settings", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	enum class TEST_ENUM : int
	{
		ONE,
		TWO,
		THREE
	};

	App::Core::Settings::SettingsSystem settings = App::Core::Settings::SettingsSystem();

	// Bool
	settings.referenceSetting( "BOOL_SETTING", true );
	CHECK( settings.get<bool>( "BOOL_SETTING" ) == true );
	settings.set( "BOOL_SETTING", false );
	CHECK( settings.get<bool>( "BOOL_SETTING" ) == false );

	// Int
	settings.referenceSetting( "INT_SETTING", 4 );
	CHECK( settings.get<int>( "INT_SETTING" ) == 4 );
	settings.set( "INT_SETTING", 8 );
	CHECK( settings.get<int>( "INT_SETTING" ) == 8 );

	// UInt
	settings.referenceSetting<uint>( "UINT_SETTING", uint( 4 ) );
	CHECK( settings.get<uint>( "UINT_SETTING" ) == 4 );
	settings.set( "UINT_SETTING", uint( 8 ) );
	CHECK( settings.get<uint>( "UINT_SETTING" ) == 8 );

	// Float
	settings.referenceSetting( "FLOAT_SETTING", 10.f );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 10.f );
	settings.set( "FLOAT_SETTING", 42.f );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 42.f );

	// String
	settings.referenceSetting( "STRING_SETTING", std::string( "Zouzou" ) );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "Zouzou" );
	settings.set( "STRING_SETTING", std::string( "Zaza" ) );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "Zaza" );

	// Enum
	settings.referenceSetting( "ENUM_SETTING", TEST_ENUM::TWO );
	CHECK( settings.get<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::TWO );
	settings.set( "ENUM_SETTING", TEST_ENUM::THREE );
	CHECK( settings.get<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::THREE );

	// Vec3f
	settings.referenceSetting( "VEC3F_SETTING", VEC3F_ZERO );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	settings.set( "VEC3F_SETTING", VEC3F_XYZ );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_XYZ );

	//// Color
	settings.referenceSetting( "COLOR_SETTING", COLOR_BLUE );
	CHECK( settings.get<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
	settings.set( "COLOR_SETTING", COLOR_GREEN );
	CHECK( settings.get<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_GREEN );

	// std::vector<std::string>
	settings.referenceSetting<std::vector<std::string>>( "STRING_VECTOR_SETTING", { "Zero", "One", "Two", "Three" } );
	CHECK( settings.get<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Two" );
	settings.set<std::vector<std::string>>( "STRING_VECTOR_SETTING", { "Zero", "One", "Six", "Three" } );
	CHECK( settings.get<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Six" );

	// Check default parameter
	settings.referenceSetting<int>( "INT_SETTING_2" );
	CHECK( settings.get<int>( "INT_SETTING_2" ) == 0 );

	// Reset
	settings.reset();
	CHECK( settings.get<int>( "INT_SETTING" ) == 4 );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 10.f );
	CHECK( settings.get<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::TWO );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	CHECK( settings.get<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
	CHECK( settings.get<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Two" );

	// Ensure that reset modify value and not ptr
	settings.set( "COLOR_SETTING", COLOR_GREEN );
	settings.reset();
	CHECK( settings.get<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
};
