/*

#include "util/serialization.hpp"
#include "util/app.hpp"
#include <app/action/application.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/core/serialization/upgrade_utility.hpp>
#include <app/core/serialization/version.hpp>
#include <app/core/settings/settings_system.hpp>
#include <app/fixture.hpp>
#include <app/serialization/serialization_system.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <filesystem>
#include <string>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
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

TEST_CASE( "VTX_APP - Serialization - Default types", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	const FilePath filepath = Util::Filesystem::getExecutableDir();
	CHECK( Test::Util::Serialization::checkSerialization( filepath ) );

	const Vec2i vec2i = Vec2i( random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec2i ) );
	const Vec2f vec2f = Vec2f( random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec2f ) );

	const Vec3i vec3i = Vec3i( random( 0, 100 ), random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec3i ) );
	const Vec3f vec3f = Vec3f( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec3f ) );
	const Vec3d vec3d = Vec3d( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec3d ) );
	const Vec3u vec3u = Vec3u( random( 0, 100 ), random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec3u ) );
	const Vec3b vec3b = Vec3b( random( 0, 1 ), random( 0, 1 ), random( 0, 1 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec3b ) );

	const Vec4i vec4i = Vec4i( random( 0, 100 ), random( 0, 100 ), random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec4i ) );
	const Vec4f vec4f = Vec4f( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec4i ) );
	const Vec4d vec4d = Vec4d( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec4d ) );
	const Vec4u vec4u = Vec4u( random( 0, 100 ), random( 0, 100 ), random( 0, 100 ), random( 0, 100 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec4u ) );
	const Vec4b vec4b = Vec4b( random( 0, 1 ), random( 0, 1 ), random( 0, 1 ), random( 0, 1 ) );
	CHECK( Test::Util::Serialization::checkSerialization( vec4b ) );

	const Mat3f mat3f = Mat3f(
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 )
	);
	CHECK( Test::Util::Serialization::checkSerialization( mat3f ) );
	const Mat43f mat43f = Mat43f(
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 )
	);
	CHECK( Test::Util::Serialization::checkSerialization( mat43f ) );
	const Mat4f mat4f = Mat4f(
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 ),
		random( 0, 100 )
	);
	CHECK( Test::Util::Serialization::checkSerialization( mat4f ) );

	const Quatf quatf = Quatf( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( quatf ) );
	const Quatd quatd = Quatd( random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ), random( 0.f, 100.f ) );
	CHECK( Test::Util::Serialization::checkSerialization( quatd ) );

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
			{
				for ( int j = 0; j < 4; j++ )
				{
					if ( std::abs( p_lhs.get()[ i ][ j ] - p_rhs.get()[ i ][ j ] ) > ( EPSILON * 10 ) )
						return false;
				}
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

	App::Fixture app;

	SERIALIZATION_SYSTEM().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZATION_SYSTEM().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const VTX::Util::JSon::Document jsonDoc = { { "CUSTOM_CLASS", SERIALIZATION_SYSTEM().serialize( custom ) } };

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

	App::Fixture app;

	SERIALIZATION_SYSTEM().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZATION_SYSTEM().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const FilePath jsonPath = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest.json";

	SERIALIZATION_SYSTEM().writeObject( jsonPath, custom );
	REQUIRE( std::filesystem::exists( jsonPath ) );

	CustomClass loadedCustom = CustomClass();
	CHECK( custom != loadedCustom );

	SERIALIZATION_SYSTEM().readObject( jsonPath, loadedCustom );
	CHECK( custom == loadedCustom );
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

TEST_CASE( "VTX_APP - Serialization - Scene", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	App::Fixture app;
	Test::Util::App::loadTestTrajectorySystem();

	const std::string systemName = App::Test::Util::App::MOLECULE_TRAJECTORY_TEST_NAME;

	App::Component::Chemistry::System & system
		= SCENE().getComponentByName<App::Component::Chemistry::System>( systemName );

	for ( atom_index_t i = system.getResidue( 1 )->getIndexFirstAtom();
		  i <= system.getResidue( 1 )->getIndexLastAtom();
		  i++ )
	{
		system.getAtom( i )->setVisible( false );
	}

	const FilePath jsonPath = Util::Filesystem::getExecutableDir() / "data/serialization/scene.vtx";

	App::Action::Application::SaveScene saveSceneAction = App::Action::Application::SaveScene( jsonPath );
	saveSceneAction.execute();

	App::Action::Application::OpenScene openSceneAction = App::Action::Application::OpenScene( jsonPath );
	openSceneAction.execute();

	App::Application::Scene & loadedScene = SCENE();

	CHECK( loadedScene.getItemCount() == 1 );
	const App::Component::Scene::SceneItemComponent & sceneItemObj
		= loadedScene.getComponentByIndex<App::Component::Scene::SceneItemComponent>( 0 );
	REQUIRE( sceneItemObj.getName() == systemName );

	const App::Component::Chemistry::System & loadedSystem
		= loadedScene.getComponentByName<App::Component::Chemistry::System>( systemName );

	CHECK( loadedSystem.getAtoms().size() == 263 );
	CHECK( !loadedSystem.getAtom( loadedSystem.getResidue( 1 )->getIndexFirstAtom() )->isVisible() );
	CHECK( !loadedSystem.getAtom( loadedSystem.getResidue( 1 )->getIndexLastAtom() )->isVisible() );
	// CHECK( system.getAtoms().size() == 113095 );
}

TEST_CASE( "VTX_APP - Serialization - Settings", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	App::Fixture app;

	SERIALIZATION_SYSTEM().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZATION_SYSTEM().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	App::Core::Settings::SettingsSystem settings;

	settings.referenceSetting( "INT_SETTING", 0 );
	settings.referenceSetting( "FLOAT_SETTING", 0.f );
	settings.referenceSetting<std::string>( "STRING_SETTING", "<default>" );
	settings.referenceSetting( "VEC3F_SETTING", VEC3F_ZERO );
	const CustomClass defaultCustomClass = CustomClass();
	settings.referenceSetting( "CUSTOM_CLASS_SETTING", defaultCustomClass );

	CHECK( settings.get<int>( "INT_SETTING" ) == 0 );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 0.f );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "<default>" );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	CHECK( settings.get<CustomClass>( "CUSTOM_CLASS_SETTING" ) == defaultCustomClass );

	settings.set( "INT_SETTING", 10 );
	settings.set( "FLOAT_SETTING", 10.f );
	settings.set<std::string>( "STRING_SETTING", "value" );
	settings.set( "VEC3F_SETTING", VEC3F_Z );
	CustomClass newCustomClass = settings.get<CustomClass>( "CUSTOM_CLASS_SETTING" );
	newCustomClass.color	   = COLOR_BLACK;
	settings.set( "CUSTOM_CLASS_SETTING", newCustomClass );

	CHECK( settings.get<int>( "INT_SETTING" ) == 10 );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 10.f );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "value" );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_Z );
	CHECK( settings.get<CustomClass>( "CUSTOM_CLASS_SETTING" ) == newCustomClass );

	const Util::JSon::BasicJSon json = SERIALIZATION_SYSTEM().serialize( settings );

	settings.reset();

	CHECK( settings.get<int>( "INT_SETTING" ) == 0 );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 0.f );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "<default>" );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_ZERO );
	CHECK( settings.get<CustomClass>( "CUSTOM_CLASS_SETTING" ) == defaultCustomClass );

	SERIALIZATION_SYSTEM().deserialize( json, settings );

	CHECK( settings.get<int>( "INT_SETTING" ) == 10 );
	CHECK( settings.get<float>( "FLOAT_SETTING" ) == 10.f );
	CHECK( settings.get<std::string>( "STRING_SETTING" ) == "value" );
	CHECK( settings.get<Vec3f>( "VEC3F_SETTING" ) == VEC3F_Z );
	CHECK( settings.get<CustomClass>( "CUSTOM_CLASS_SETTING" ) == newCustomClass );

	// Setting version check
	App::Core::Settings::SettingsSystem settingsV0;
	settingsV0.referenceSetting<int>( "SETTING_1", 0 );
	settingsV0.referenceSetting<int>( "SETTING_2", 0 );
	settingsV0.referenceSetting<std::string>( "SETTING_3", "<default>" );

	App::Core::Settings::SettingsSystem settingsV1;
	settingsV1.referenceSetting<int>( "SETTING_1", 0 ); // This setting has been kept
	// settingsV1.referenceSetting<int>( "SETTING_2" ); // This setting has been removed
	settingsV1.referenceSetting<float>( "SETTING_3", 0.f );						// This setting has changed type
	settingsV1.referenceSetting<std::string>( "SETTING_4", "<set_4_default>" ); // This setting has been added

	const FilePath jsonPath_0_0_0
		= Util::Filesystem::getExecutableDir() / "data/serialization/json_settings_0_0_0.json";

	App::Serialization::SerializationSystem::Reader reader = { SERIALIZATION_SYSTEM(), jsonPath_0_0_0, &settingsV1 };
	reader.read();

	CHECK( settingsV1.get<int>( "SETTING_1" ) == 10 );						  // Loaded
	CHECK( !settingsV1.contains( "SETTING_2" ) );							  // Skipped
	CHECK( settingsV1.get<float>( "SETTING_3" ) == 0.f );					  // Don't change (type issue)
	CHECK( settingsV1.get<std::string>( "SETTING_4" ) == "<set_4_default>" ); // Don't change (doesn't exists before)

	SERIALIZATION_SYSTEM().registerUpgrade<App::Core::Settings::SettingsSystem>(
		{ 1, 0, 0 },
		[]( Util::JSon::Object & p_json, App::Core::Settings::SettingsSystem & p_settings )
		{ p_json[ "MAP" ][ "SETTING_3" ] = 12.f; }
	);

	reader.read();

	CHECK( settingsV1.get<float>( "SETTING_3" ) == 12.f ); // Type issue corrected by upgrade.
}

TEST_CASE( "VTX_APP - Serialization - Upgrade", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using CustomClass = Test::Util::Serialization::CustomClass;

	App::Fixture app;

	SERIALIZATION_SYSTEM().registerSerializationFunction<CustomClass>( &CustomClass::serialize );
	SERIALIZATION_SYSTEM().registerDeserializationFunction<CustomClass>( &CustomClass::deserialize );

	SERIALIZATION_SYSTEM().registerUpgrade<CustomClass>( { 0, 1, 0 }, &CustomClass::upgrade_0_0_0_to_0_1_0 );
	SERIALIZATION_SYSTEM().registerUpgrade<CustomClass>( { 1, 0, 0 }, &CustomClass::upgrade_0_1_0_to_1_0_0 );

	const CustomClass custom
		= CustomClass( COLOR_BLUE, "strValue", 42, { 1.f, 2.f, 3.f }, CustomClass::CustomEnum::TWO );

	const FilePath jsonPath_0_1_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_1_0.json";
	CustomClass	   loadedCustom_0_1_0 = CustomClass();

	SERIALIZATION_SYSTEM().readObject( jsonPath_0_1_0, loadedCustom_0_1_0 );

	CHECK( custom == loadedCustom_0_1_0 );

	const FilePath jsonPath_0_0_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_0_0_0.json";
	CustomClass	   loadedCustom_0_0_0 = CustomClass();

	SERIALIZATION_SYSTEM().readObject( jsonPath_0_0_0, loadedCustom_0_0_0 );

	CHECK( loadedCustom_0_0_0.color == custom.color );
	CHECK( loadedCustom_0_0_0.enumValue == custom.enumValue );
	CHECK( loadedCustom_0_0_0.strValue == custom.strValue );

	const FilePath jsonPath_1_2_0	  = Util::Filesystem::getExecutableDir() / "data/serialization/jsonTest_1_2_0.json";
	CustomClass	   loadedCustom_1_2_0 = CustomClass();

	try
	{
		SERIALIZATION_SYSTEM().readObject( jsonPath_0_0_0, loadedCustom_0_0_0 );
	}
	catch ( const IOException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "More recent file check fail : {}", p_e.what() );
		CHECK( false );
	}
}
*/
