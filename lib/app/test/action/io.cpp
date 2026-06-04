#include <app/action/io.hpp>
#include <app/action/visibility.hpp>
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <fmt/format.h>

TEST_CASE( "VTX_APP - Action - Io - WriteSelection", "[integration][visibility][action][write]" )
{
	using namespace VTX;
	App::Fixture app;

	app.loadSystem();

	const auto [ ent, topology, visibility ]
		= App::ECS::getFirstEntityWithComponents<VTX::Core::Struct::Topology, VTX::App::System::Visibility>();

	visibility.atoms.set( 0, false );
	uint initialAtomCount = topology.getAtomCount(), rewriteAtomCount = 0;

	VTX::FilePath outPath { VTX::Util::Filesystem::getExecutableDir() / "out" / "1aga.bcif" };

	VTX::App::ACTION().execute<VTX::App::Action::IO::WriteVisible>( outPath );

	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>( outPath );

	for ( auto entt : VTX::App::REG().view<VTX::Core::Struct::Topology, VTX::App::System::Visibility>() )
	{
		if ( entt == ent )
		{
			continue;
		}
		auto newTopol = App::REG().get<VTX::Core::Struct::Topology>( entt );
		CHECK( newTopol.getAtomCount() == ( initialAtomCount - 1 ) );
	}
}
