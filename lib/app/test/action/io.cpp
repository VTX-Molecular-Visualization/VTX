#include <app/action/io.hpp>
#include <app/action/visibility.hpp>
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

TEST_CASE( "VTX_APP - Action - Queue owns explicitly copied names", "[action][queue]" )
{
	VTX::App::Fixture app;
	std::string		  result;

	struct ReadName
	{
		std::string & result;

		void execute( const std::string_view p_name ) { result = p_name; }
	};

	struct ReadOptionalName
	{
		std::string & result;

		void execute( const std::optional<std::string_view> p_name ) { result = p_name.value_or( "missing" ); }
	};

	std::string					   source = "original";
	VTX::App::Action::QueuedAction action( ReadName { result }, std::string( source ) );
	source.assign( "modified" );
	action.execute();
	CHECK( result == "original" );

	VTX::App::Action::QueuedAction optionalAction( ReadOptionalName { result }, std::optional<std::string>( source ) );
	source.assign( "replaced" );
	optionalAction.execute();
	CHECK( result == "modified" );

	VTX::App::Action::QueuedAction emptyAction( ReadOptionalName { result }, std::optional<std::string>() );
	emptyAction.execute();
	CHECK( result == "missing" );
}

TEST_CASE( "VTX_APP - Action - Queue can move a copied const argument", "[action][queue]" )
{
	VTX::App::Fixture app;
	std::string		  result;

	struct ConsumeName
	{
		std::string & result;

		void execute( std::string && p_name ) { result = std::move( p_name ); }
	};

	const std::string			   source = "original";
	VTX::App::Action::QueuedAction action( ConsumeName { result }, source );
	action.execute();
	CHECK( result == source );
}

TEST_CASE( "VTX_APP - Action - Invalid trajectory association finishes", "[action][io]" )
{
	VTX::App::Fixture app;

	SECTION( "Path" )
	{
		VTX::App::Action::IO::AssociateTrajectory action;
		action.execute( VTX::FilePath( "unused" ), VTX::InvalidEntity );
		action.wait();
	}
	SECTION( "String" )
	{
		VTX::App::Action::IO::AssociateTrajectory action;
		action.execute( std::string( "unused" ), VTX::InvalidEntity );
		action.wait();
	}
}

TEST_CASE( "VTX_APP - Action - Io - WriteSelection", "[integration][visibility][action][write]" )
{
	using namespace VTX;
	App::Fixture app;

	app.loadSystem();

	const auto [ ent, topology, visibility ]
		= App::ECS::getFirstEntityWithComponents<VTX::Core::Struct::Topology, VTX::App::System::Visibility>();

	VTX::App::Action::Visibility::SetVisible<Core::Struct::E_SYSTEM_ITEM::ATOM> setVisible;
	VTX::App::ACTION().execute( setVisible, ent, Core::Struct::IndexRangeList { { 0, 1 } }, false );
	const uint initialAtomCount = topology.getAtomCount();
	uint	   rewriteAtomCount = 0;

	const VTX::FilePath outDir = VTX::Util::Filesystem::getExecutableDir() / "out";
	std::filesystem::create_directories( outDir );
	VTX::FilePath outPath { outDir / "1aga.bcif" };

	{
		VTX::App::Action::IO::WriteVisible a;
		VTX::App::ACTION().execute( a, outPath );
	}

	{
		VTX::App::Action::IO::LoadSystem a;
		VTX::App::ACTION().execute( a, outPath );
	}

	for ( auto entt : VTX::App::REG().view<VTX::Core::Struct::Topology>() )
	{
		if ( entt == ent )
		{
			continue;
		}
		auto newTopol = App::REG().get<VTX::Core::Struct::Topology>( entt );
		CHECK( newTopol.getAtomCount() == ( initialAtomCount - 1 ) );
	}
}
