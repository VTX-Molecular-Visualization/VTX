#include <app/settings/settings_manager.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/color/rgba.hpp>
#include <util/constants.hpp>

TEST_CASE( "VTX_APP - Settings", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;
	using namespace VTX::App::Settings;

	enum class TEST_ENUM : int
	{
		ONE,
		TWO,
		THREE
	};

	SettingsManager settings;

	// Bool
	settings.add( "BOOL_SETTING", true );
	CHECK( settings.getValue<bool>( "BOOL_SETTING" ) == true );
	settings.setValue( "BOOL_SETTING", false );
	CHECK( settings.getValue<bool>( "BOOL_SETTING" ) == false );

	// Int
	settings.add( "INT_SETTING", 4 );
	CHECK( settings.getValue<int>( "INT_SETTING" ) == 4 );
	settings.setValue( "INT_SETTING", 8 );
	CHECK( settings.getValue<int>( "INT_SETTING" ) == 8 );

	// UInt
	settings.add<uint>( "UINT_SETTING", uint( 4 ) );
	CHECK( settings.getValue<uint>( "UINT_SETTING" ) == 4 );
	settings.setValue( "UINT_SETTING", uint( 8 ) );
	CHECK( settings.getValue<uint>( "UINT_SETTING" ) == 8 );

	// Float
	settings.add( "FLOAT_SETTING", 10.f );
	CHECK( settings.getValue<float>( "FLOAT_SETTING" ) == 10.f );
	settings.setValue( "FLOAT_SETTING", 42.f );
	CHECK( settings.getValue<float>( "FLOAT_SETTING" ) == 42.f );

	// String
	settings.add( "STRING_SETTING", std::string( "Zouzou" ) );
	CHECK( settings.getValue<std::string>( "STRING_SETTING" ) == "Zouzou" );
	settings.setValue( "STRING_SETTING", std::string( "Zaza" ) );
	CHECK( settings.getValue<std::string>( "STRING_SETTING" ) == "Zaza" );

	// Enum
	settings.add( "ENUM_SETTING", TEST_ENUM::TWO );
	CHECK( settings.getValue<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::TWO );
	settings.setValue( "ENUM_SETTING", TEST_ENUM::THREE );
	CHECK( settings.getValue<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::THREE );

	// Vec3f
	settings.add( "VEC3F_SETTING", VEC3F_ZERO );
	CHECK( settings.getValue<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	settings.setValue( "VEC3F_SETTING", VEC3F_XYZ );
	CHECK( settings.getValue<Vec3f>( "VEC3F_SETTING" ) == VEC3F_XYZ );

	//// Color
	settings.add( "COLOR_SETTING", COLOR_BLUE );
	CHECK( settings.getValue<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
	settings.setValue( "COLOR_SETTING", COLOR_GREEN );
	CHECK( settings.getValue<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_GREEN );

	// std::vector<std::string>
	settings.add<std::vector<std::string>>( "STRING_VECTOR_SETTING", { "Zero", "One", "Two", "Three" } );
	CHECK( settings.getValue<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Two" );
	settings.setValue<std::vector<std::string>>( "STRING_VECTOR_SETTING", { "Zero", "One", "Six", "Three" } );
	CHECK( settings.getValue<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Six" );

	// Check default parameter
	settings.add<int>( "INT_SETTING_2" );
	CHECK( settings.getValue<int>( "INT_SETTING_2" ) == 0 );

	// Reset
	settings.reset();
	CHECK( settings.getValue<int>( "INT_SETTING" ) == 4 );
	CHECK( settings.getValue<float>( "FLOAT_SETTING" ) == 10.f );
	CHECK( settings.getValue<TEST_ENUM>( "ENUM_SETTING" ) == TEST_ENUM::TWO );
	CHECK( settings.getValue<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	CHECK( settings.getValue<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
	CHECK( settings.getValue<std::vector<std::string>>( "STRING_VECTOR_SETTING" )[ 2 ] == "Two" );

	// Ensure that reset modify value and not ptr
	settings.setValue( "COLOR_SETTING", COLOR_GREEN );
	settings.reset();
	CHECK( settings.getValue<Util::Color::Rgba>( "COLOR_SETTING" ) == COLOR_BLUE );
};
