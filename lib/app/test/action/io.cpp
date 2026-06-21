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

	VTX::App::Action::Visibility::SetVisible<Core::Struct::E_SYSTEM_ITEM::ATOM> setVisible;
	VTX::App::ACTION().execute( setVisible, ent, Core::Struct::IndexRangeList { { 0, 1 } }, false );
	uint initialAtomCount = topology.getAtomCount(), rewriteAtomCount = 0;

	VTX::FilePath outDir = VTX::Util::Filesystem::getExecutableDir() / "out";
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
