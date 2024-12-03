#include "util/app.hpp"
#include "util/selection.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/selection/system_data.hpp>
#include <app/fixture.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/interpretor.hpp>
#include <util/math/range_list.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - VTX API Selection Tests", "[integration]" )
{
	return; // TODO : when selection save is restored
	using namespace VTX;
	using SelectionUtil = App::Test::Util::Selection;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	interpretor.init();

	App::Test::Util::PythonFixture::loadSystem( "1AGA.mmtf" );
	App::Test::Util::PythonFixture::loadSystem( "4HHB.pdb" );
	App::Test::Util::PythonFixture::loadSystem( "8QHQ.pdb" );

	App::Component::Chemistry::System & mol4hhb
		= App::SCENE().getComponentByName<App::Component::Chemistry::System>( "4HHB" );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_1",
		"select( mol_n='4HHB' )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB" ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_i_1",
		"select( mol_i=0 )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "1AGA" ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_chain_n_1",
		"select( mol_n='4HHB', chain_n='A' )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_chain_i_1",
		"select( mol_n='4HHB', chain_i=1 )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 1 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_chain_n_1",
		"select( chain_n='A' )",
		SelectionUtil::createSelection( { *SelectionUtil::generateSystemData( "4HHB", { 0 } ),
										  *SelectionUtil::generateSystemData( "1AGA", { 0 } ),
										  *SelectionUtil::generateSystemData( "8QHQ", { 0, 6 } ) } )
	) );

	std::unique_ptr<App::Application::Selection::SelectionData> allHistidineOn4HHB = SelectionUtil::generateSystemData(
		"4HHB",
		{},
		{
			20 - 1,
			45 - 1,
			50 - 1,
			58 - 1,
			72 - 1,
			87 - 1,
			89 - 1,
			103 - 1,
			112 - 1,
			122 - 1,
			141 + 2 - 1,
			141 + 63 - 1,
			141 + 77 - 1,
			141 + 92 - 1,
			141 + 97 - 1,
			141 + 116 - 1,
			141 + 117 - 1,
			141 + 143 - 1,
			141 + 146 - 1,
			141 + 146 + 20 - 1,
			141 + 146 + 45 - 1,
			141 + 146 + 50 - 1,
			141 + 146 + 58 - 1,
			141 + 146 + 72 - 1,
			141 + 146 + 87 - 1,
			141 + 146 + 89 - 1,
			141 + 146 + 103 - 1,
			141 + 146 + 112 - 1,
			141 + 146 + 122 - 1,
			141 + 146 + 141 + 2 - 1,
			141 + 146 + 141 + 63 - 1,
			141 + 146 + 141 + 77 - 1,
			141 + 146 + 141 + 92 - 1,
			141 + 146 + 141 + 97 - 1,
			141 + 146 + 141 + 116 - 1,
			141 + 146 + 141 + 117 - 1,
			141 + 146 + 141 + 143 - 1,
			141 + 146 + 141 + 146 - 1,
		}
	);

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_res_n_1",
		"select( mol_n='4HHB', res_n='HIS' )",
		SelectionUtil::createSelection( allHistidineOn4HHB )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_res_n_1", "select( mol_n='4HHB', res_n='H' )", SelectionUtil::createSelection( allHistidineOn4HHB )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_res_n_1",
		"select( mol_n='4HHB', res_n='Histidine' )",
		SelectionUtil::createSelection( allHistidineOn4HHB )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_n_chain_res_1",
		"select( mol_n='4HHB', chain_n='B', res_i=4 )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", {}, { 144 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_chain_res_atom_1",
		"select( mol_n='4HHB', chain_n='A', res_i=10, atom_t='C')",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", {}, {}, { 66, 67, 69, 70, 71 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_mol_chain_res_atom_2",
		"select( mol_n='4HHB', chain_n='A', res_i=10, atom_n='C')",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", {}, {}, { 67 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_addition_1",
		"select( mol_n='4HHB', chain_n='A' ) + select( mol_n='4HHB', chain_n='C')",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0, 2 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_substract_1",
		"select(mol_n='4HHB', chain_n='A' ) - select( mol_n='4HHB', chain_n='B')",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_intersect_1",
		"intersect(select( mol_n='4HHB' ), select( chain_n='A' ))",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_intersect_1",
		"intersect(select( mol_n='4HHB' ), select( res_n='HIS' ))",
		SelectionUtil::createSelection( allHistidineOn4HHB )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_exclusive_1",
		"exclusive( (select( mol_n='4HHB' ) - select(  mol_n='4HHB', res_n='HIS' )), select( mol_n='4HHB', "
		"res_n='HIS' "
		") )",
		SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB" ) )
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_empty_1", "select( mol_n='Nimportequoi', chain_i=1 )", App::Application::Selection::Selection()
	) );

	CHECK( SelectionUtil::checkSelection(
		"test_empty_2", "select( mol_n='4HHB', res_n='his' )", SelectionUtil::createSelection( allHistidineOn4HHB )
	) );

	// CHECK( SelectionUtil::checkSelection(
	//	"test_mol_n_chain_str_i_1", "select( mol_n='4HHB', chain_i='1' )",
	//PythonFixture::Application::Selection::Selection() ) ); // NO => manage param as str

	// interpretor.runCommand( "select( mol_n='4HHB', res_i=range(0, 100) )" );
};
