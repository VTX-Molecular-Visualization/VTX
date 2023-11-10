#include "util/app.hpp"
#include <app/application/ecs/registry_manager.hpp>
#include <app/application/scene.hpp>
#include <app/application/selection/molecule_data.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Selection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Application::Scene & scene = VTXApp::get().getScene();

	Test::Util::App::loadMolecule( "8OIT.mmtf" );
	const Component::Scene::Selectable & selectableMol1
		= SCENE().getComponentByName<Component::Scene::Selectable>( "8OIT" );

	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	const Component::Scene::Selectable & selectableMol2
		= SCENE().getComponentByName<Component::Scene::Selectable>( "1AGA" );

	CHECK( CURRENT_SELECTION().isEmpty() );

	Application::Selection::MoleculeData & molSelData1
		= CURRENT_SELECTION().select<Application::Selection::MoleculeData>( selectableMol1 );

	CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
}
