#include <app/action/io.hpp>
#include <app/action/material.hpp>
#include <app/action/preset.hpp>
#include <app/fixture.hpp>
#include <app/generic/file.hpp>
#include <app/generic/name.hpp>
#include <app/generic/readonly.hpp>
#include <app/helper/preset.hpp>
#include <app/scene/color_layout.hpp>
#include <app/scene/material.hpp>
#include <app/serialization/renderer.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <optional>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <renderer/representation.hpp>
#include <string>
#include <string_view>
#include <util/action_manager.hpp>
#include <util/constants.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/json.hpp>
#include <vector>

namespace
{
	using namespace VTX;
	using namespace VTX::App;

	template<typename T>
	void checkBuiltinPresets( const std::vector<std::string_view> & p_names )
	{
		for ( const std::string_view name : p_names )
		{
			const std::optional<Entity> entity = Helper::Preset::getByName<T>( name );
			REQUIRE( entity.has_value() );
			CHECK( REG().all_of<Generic::ReadOnly, Generic::File>( *entity ) );
			CHECK( std::filesystem::exists( REG().get<Generic::File>( *entity ).path ) );
		}
	}

	template<typename T>
	void checkRoundTrip()
	{
		auto view = REG().view<Generic::Name, T>();
		REQUIRE( view.begin() != view.end() );
		for ( const Entity entity : view )
		{
			const Util::Json::Document json = view.template get<T>( entity );
			const T					   data = json.get<T>();
			CHECK( Util::Json::Document( data ) == json );
		}
	}

	void writePreset(
		const FilePath &	   p_path,
		const uint		   p_version,
		const std::string_view p_name,
		const std::string_view p_type = Helper::Preset::TYPE_NAME<Renderer::Representation>
	)
	{
		Util::Json::Document document;
		document[ "version" ] = p_version;
		document[ "type" ]	  = p_type;
		document[ "name" ]	  = p_name;
		document[ "data" ]	  = Renderer::Representation {};
		Util::Json::write( p_path, document );
	}
} // namespace

TEST_CASE( "VTX_APP - Preset - Builtin presets", "[preset]" )
{
	App::Fixture app;

	checkBuiltinPresets<Renderer::Representation>(
		{ "Sticks", "Sticks and Ribbons", "Balls and sticks", "Van der Waals", "Ribbons", "SES" }
	);
	checkBuiltinPresets<Renderer::Color::Layout>( { "JMol", "CPK", "Rasmol" } );
	checkBuiltinPresets<Renderer::GraphicsConfig>( { "Default", "Presentation", "Illustration", "PBR Metallic" } );
	checkBuiltinPresets<Renderer::Material>( { "Default", "Matte", "Plastic", "Polished metal", "Brushed metal" } );

	CHECK(
		REG().get<Generic::Name>( *Helper::Preset::getDefault<Renderer::Representation>() ).name
		== "Sticks and Ribbons"
	);
	CHECK( REG().get<Generic::Name>( *Helper::Preset::getDefault<Renderer::Color::Layout>() ).name == "JMol" );
	CHECK( REG().get<Generic::Name>( *Helper::Preset::getDefault<Renderer::GraphicsConfig>() ).name == "Default" );

	const Entity brushedMetal = *Helper::Preset::getByName<Renderer::Material>( "Brushed metal" );
	CHECK( REG().get<Renderer::Material>( brushedMetal ).metallic == 1.f );

	const Entity jmol = *Helper::Preset::getByName<Renderer::Color::Layout>( "JMol" );
	const Renderer::Color::Layout & layout = REG().get<Renderer::Color::Layout>( jmol );
	CHECK( layout.getAtomColor( 6 ) == Util::Color::Rgba( 144, 144, 144 ) );
	CHECK( layout.getChainColor( 1 ) == Util::Color::Rgba( 192, 208, 255 ) );
}

TEST_CASE( "VTX_APP - Preset - Round trip", "[preset]" )
{
	App::Fixture app;

	checkRoundTrip<Renderer::Representation>();
	checkRoundTrip<Renderer::Color::Layout>();
	checkRoundTrip<Renderer::GraphicsConfig>();
	checkRoundTrip<Renderer::Material>();
}

TEST_CASE( "VTX_APP - Preset - Load directory", "[preset]" )
{
	App::Fixture app;

	const FilePath directory = std::filesystem::temp_directory_path() / "vtx_app_test_presets";
	std::filesystem::remove_all( directory );
	std::filesystem::create_directories( directory );

	writePreset( directory / "valid.json", 1, "Test preset" );
	writePreset( directory / "newer.json", Helper::Preset::VERSION<Renderer::Representation> + 1, "Newer preset" );
	writePreset( directory / "duplicate.json", 1, "Sticks" );
	writePreset(
		directory / "wrong_type.json", 1, "Wrong type preset", Helper::Preset::TYPE_NAME<Renderer::Material>
	);
	Util::Filesystem::writeFile( directory / "invalid.json", "{ \"version\": " );
	writePreset( directory / "ignored.txt", 1, "Ignored preset" );

	ACTION().execute<Action::Preset::LoadDirectory<Renderer::Representation>>( directory, false );

	const std::optional<Entity> valid = Helper::Preset::getByName<Renderer::Representation>( "Test preset" );
	REQUIRE( valid.has_value() );
	CHECK( not REG().all_of<Generic::ReadOnly>( *valid ) );
	CHECK( REG().get<Generic::File>( *valid ).path == directory / "valid.json" );

	CHECK( not Helper::Preset::exists<Renderer::Representation>( "Newer preset" ) );
	CHECK( not Helper::Preset::exists<Renderer::Representation>( "Ignored preset" ) );
	CHECK( not Helper::Preset::exists<Renderer::Representation>( "Wrong type preset" ) );
	CHECK( REG().all_of<Generic::ReadOnly>( *Helper::Preset::getByName<Renderer::Representation>( "Sticks" ) ) );

	ACTION().execute<Action::Preset::LoadDirectory<Renderer::Representation>>( directory / "missing", false );

	std::filesystem::remove_all( directory );
}

TEST_CASE( "VTX_APP - Preset - Import", "[preset]" )
{
	App::Fixture app;

	const FilePath directory = std::filesystem::temp_directory_path() / "vtx_app_test_presets_import";
	std::filesystem::remove_all( directory );
	std::filesystem::create_directories( directory );

	writePreset( directory / "imported.json", 1, "Sticks" );
	writePreset( directory / "opened.json", 1, "Opened preset" );
	writePreset( directory / "unknown.json", 1, "Unknown type preset", "unknown" );
	writePreset( directory / "newer.json", Helper::Preset::VERSION<Renderer::Representation> + 1, "Newer preset" );

	Util::Json::Document layout;
	layout[ "version" ] = Helper::Preset::VERSION<Renderer::Color::Layout>;
	layout[ "type" ]	= Helper::Preset::TYPE_NAME<Renderer::Color::Layout>;
	layout[ "name" ]	= "Imported layout";
	layout[ "data" ]	= Renderer::Color::Layout {};
	Util::Json::write( directory / "layout.json", layout );

	ACTION().execute<Action::Preset::Import>( directory / "imported.json" );
	ACTION().execute<Action::Preset::Import>( directory / "layout.json" );
	ACTION().execute<Action::IO::Open>( directory / "opened.json" );
	ACTION().execute<Action::Preset::Import>( directory / "unknown.json" );
	ACTION().execute<Action::Preset::Import>( directory / "newer.json" );

	const std::optional<Entity> imported = Helper::Preset::getByName<Renderer::Representation>( "Sticks (2)" );
	REQUIRE( imported.has_value() );
	CHECK( not REG().all_of<Generic::ReadOnly>( *imported ) );

	const FilePath importedPath = REG().get<Generic::File>( *imported ).path;
	CHECK( importedPath.parent_path() == SESSION().getRepresentationsDir() );
	CHECK( Util::Json::read( importedPath ).at( "name" ).get<std::string>() == "Sticks (2)" );

	const std::optional<Entity> opened = Helper::Preset::getByName<Renderer::Representation>( "Opened preset" );
	REQUIRE( opened.has_value() );
	const FilePath openedPath = REG().get<Generic::File>( *opened ).path;

	const std::optional<Entity> importedLayout = Helper::Preset::getByName<Renderer::Color::Layout>( "Imported layout" );
	REQUIRE( importedLayout.has_value() );
	CHECK( ECS::getFirstComponent<App::Scene::ColorLayout>().preset == *importedLayout );
	const FilePath layoutPath = REG().get<Generic::File>( *importedLayout ).path;
	CHECK( layoutPath.parent_path() == SESSION().getColorLayoutsDir() );

	CHECK( not Helper::Preset::exists<Renderer::Representation>( "Unknown type preset" ) );
	CHECK( not Helper::Preset::exists<Renderer::Representation>( "Newer preset" ) );
	CHECK( not std::filesystem::exists( SESSION().getRepresentationsDir() / "newer.json" ) );

	std::filesystem::remove( importedPath );
	std::filesystem::remove( openedPath );
	std::filesystem::remove( layoutPath );
	std::filesystem::remove_all( directory );
}

TEST_CASE( "VTX_APP - Preset - Color layout entries", "[preset]" )
{
	const Util::Json::Document json = Util::Json::Document::parse( R"({
		"atoms": { "C": "#112233FF", "cl": "#445566FF", "XX": "#000000FF" },
		"chains": { "B": "#778899FF", "HETATM_B": "#AABBCCFF" },
		"ribbons": { "STRAND": "#FFC800FF" },
		"custom": [ "#010203FF" ]
	})" );

	const Renderer::Color::Layout layout = json.get<Renderer::Color::Layout>();

	CHECK( layout.getAtomColor( 6 ) == Util::Color::Rgba( 0x11, 0x22, 0x33 ) );
	CHECK( layout.getAtomColor( 17 ) == Util::Color::Rgba( 0x44, 0x55, 0x66 ) );
	CHECK( layout.getAtomColor( 7 ) == COLOR_WHITE );
	CHECK( layout.getChainColor( 2 ) == Util::Color::Rgba( 0x77, 0x88, 0x99 ) );
	CHECK( layout.colors[ Renderer::Color::getColorIndex( "B", true ) ] == Util::Color::Rgba( 0xAA, 0xBB, 0xCC ) );
	CHECK(
		layout.colors[ Renderer::Color::getColorIndex( Core::ChemDB::SecondaryStructure::TYPE::STRAND ) ]
		== Util::Color::Rgba( 255, 200, 0 )
	);
	CHECK( layout.getCustomColor( 0 ) == Util::Color::Rgba( 1, 2, 3 ) );
	CHECK( layout.getCustomColor( 1 ) == COLOR_WHITE );
}

TEST_CASE( "VTX_APP - Preset - Read-only", "[preset]" )
{
	App::Fixture app;

	const Entity sticks = *Helper::Preset::getByName<Renderer::Representation>( "Sticks" );

	Action::Preset::Rename<Renderer::Representation> rename;
	CHECK_THROWS_AS( Util::ActionManager::run( rename, sticks, std::string_view( "Renamed" ) ), VTXException );
	CHECK( REG().get<Generic::Name>( sticks ).name == "Sticks" );

	Action::Preset::Delete<Renderer::Representation> remove;
	CHECK_THROWS_AS( Util::ActionManager::run( remove, sticks ), VTXException );
	CHECK( REG().valid( sticks ) );

	ACTION().execute<Action::Preset::Duplicate<Renderer::Representation>>(
		sticks, std::optional<std::string_view>( "Sticks copy" )
	);
	const std::optional<Entity> copy = Helper::Preset::getByName<Renderer::Representation>( "Sticks copy" );
	REQUIRE( copy.has_value() );
	CHECK( not REG().any_of<Generic::ReadOnly, Generic::File>( *copy ) );

	ACTION().execute<Action::Preset::Rename<Renderer::Representation>>( *copy, std::string_view( "My sticks" ) );
	CHECK( REG().get<Generic::Name>( *copy ).name == "My sticks" );

	ACTION().execute<Action::Preset::Delete<Renderer::Representation>>( *copy );
	CHECK( not REG().valid( *copy ) );
}

TEST_CASE( "VTX_APP - Preset - Default values", "[preset]" )
{
	const Renderer::Color::Layout layout {};
	for ( const Util::Color::Rgba & color : layout.colors )
	{
		REQUIRE( color == COLOR_WHITE );
	}

	const Renderer::GraphicsConfig config {};
	CHECK( config.ssao.has_value() == Renderer::ACTIVE_SSAO_DEFAULT );
	CHECK( config.outline.has_value() == Renderer::ACTIVE_OUTLINE_DEFAULT );
	CHECK( config.fog.has_value() == Renderer::ACTIVE_FOG_DEFAULT );
	CHECK( config.chromaticAberration.has_value() == Renderer::ACTIVE_CHROMAB_DEFAULT );
	CHECK( config.pixelize.has_value() == Renderer::ACTIVE_PIXELIZE_DEFAULT );
	CHECK( config.crt.has_value() == Renderer::ACTIVE_CRT_DEFAULT );
	CHECK( config.selection.has_value() == Renderer::ACTIVE_SELECTION_DEFAULT );
	CHECK( config.shading.colorLight == Renderer::COLOR_LIGHT_DEFAULT );
	CHECK( config.selection->color == Renderer::COLOR_SELECTION_DEFAULT );

	const Renderer::Representation representation {};
	CHECK( representation.radiusCylinder == Renderer::RADIUS_CYLINDER_DEFAULT );
	CHECK( representation.sesComputeMode == Renderer::SES_COMPUTE_MODE_DEFAULT );

	const Util::Json::Document empty = Util::Json::Document::object();
	CHECK( Util::Json::Document( empty.get<Renderer::GraphicsConfig>() ) == Util::Json::Document( config ) );
	CHECK( Util::Json::Document( empty.get<Renderer::Representation>() ) == Util::Json::Document( representation ) );
	CHECK( Util::Json::Document( empty.get<Renderer::Color::Layout>() ) == Util::Json::Document( layout ) );
}

TEST_CASE( "VTX_APP - Preset - Scene material", "[preset]" )
{
	App::Fixture app;

	const Entity defaultMaterial = *Helper::Preset::getDefault<Renderer::Material>();
	CHECK( ECS::getFirstComponent<App::Scene::Material>().preset == defaultMaterial );

	const Entity brushedMetal = *Helper::Preset::getByName<Renderer::Material>( "Brushed metal" );
	ACTION().execute<Action::Preset::Apply<Renderer::Material>>( brushedMetal );
	CHECK( ECS::getFirstComponent<App::Scene::Material>().preset == brushedMetal );

	ACTION().execute<Action::Material::Change<Renderer::E_MATERIAL_VALUES::ROUGHNESS, float>>( brushedMetal, 0.25f );
	CHECK( REG().get<Renderer::Material>( brushedMetal ).roughness == 0.25f );

	const Entity matte = *Helper::Preset::getByName<Renderer::Material>( "Matte" );
	ACTION().execute<Action::Material::Change<Renderer::E_MATERIAL_VALUES::ALBEDO_TEXTURE, FilePath>>(
		matte, FilePath( "albedo.png" )
	);
	REQUIRE( REG().get<Renderer::Material>( matte ).albedoTexture.has_value() );
	CHECK( REG().get<Renderer::Material>( matte ).albedoTexture->path == "albedo.png" );

	ACTION().execute<Action::Material::Change<Renderer::E_MATERIAL_VALUES::ALBEDO_TEXTURE, FilePath>>(
		matte, FilePath {}
	);
	CHECK( not REG().get<Renderer::Material>( matte ).albedoTexture.has_value() );
}
