#include "util/app.hpp"
#include "util/filesystem.hpp"
#include "util/logger.hpp"
#include "util/selection.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/fixture.hpp>
#include <app/selection/selection_manager.hpp>
#include <app/selection/system_data.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/interpretor.hpp>
#include <util/math/range_list.hpp>
//
#include <app/python_binding/interpretor.hpp>
namespace Test
{
	void loadSystem( const char * p_filename )
	{
		using namespace VTX;
		const FilePath systemPath = VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename;
		VTX::App::Action::Scene::LoadSystem openAction = VTX::App::Action::Scene::LoadSystem( systemPath );
		openAction.execute();
	}
} // namespace Test
using AsyncJobResult = VTX::App::PythonBinding::Interpretor::AsyncJobResult;

TEST_CASE( "VTX_PYTHON_BINDING - VTX API Selection return types", "[app][python][integration][types]" )
{
	/**
	 * @brief We check that the python return types of the select return object are those expected (those from the
	 * PythonBinding::API)
	 */
	using namespace VTX;
	using SelectionUtil = App::Test::Util::Selection;
	App::Fixture app;

	std::future<AsyncJobResult> _future;
	Test::loadSystem( "1AGA.mmtf" );
	try
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getAtoms()", _future );
		auto str = _future.get();
		CHECK( str.resultStr.find( "CollectionAtom" ) != str.resultStr.npos );
	}
	catch ( CommandException & e )
	{
		VTX_ERROR( "{}", e.what() );
		CHECK( false );
	}
	try
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getResidues()", _future );
		auto str = _future.get();
		CHECK( str.resultStr.find( "CollectionResidue" ) != str.resultStr.npos );
	}
	catch ( CommandException & e )
	{
		VTX_ERROR( "{}", e.what() );
		CHECK( false );
	}
	try
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getChains()", _future );
		auto str = _future.get();
		CHECK( str.resultStr.find( "CollectionChain" ) != str.resultStr.npos );
	}
	catch ( CommandException & e )
	{
		VTX_ERROR( "{}", e.what() );
		CHECK( false );
	}
	try
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getSystems()", _future );
		auto str = _future.get();
		CHECK( str.resultStr.find( "CollectionSystem" ) != str.resultStr.npos );
	}
	catch ( CommandException & e )
	{
		VTX_ERROR( "{}", e.what() );
		CHECK( false );
	}
}
TEST_CASE( "VTX_PYTHON_BINDING - VTX API Collection crash", "[app][python][integration][collection]" )
{
	using namespace VTX;
	using SelectionUtil = App::Test::Util::Selection;
	App::Fixture app;

	Test::loadSystem( "1AGA.mmtf" );
	std::future<AsyncJobResult> _future;

	try
	{
		INTERPRETOR().runCommand( "len(select(system_names='1AGA').getAtoms())", _future );
		auto str = _future.get();
		CHECK( str.resultStr == "126" );
	}
	catch ( ... )
	{
		CHECK( false );
	}
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getAtoms()[100]", _future );
		_future.wait();
	}
	CHECK( _future.get().success == true );
	{
		INTERPRETOR().runCommand( "select(system_names='1AGA').getAtoms()[1000]", _future );
		_future.wait();
	}
	CHECK( _future.get().success == false );
}
TEST_CASE( "VTX_PYTHON_BINDING - VTX API Selection Tests", "[app][python][integration][selection]" )
{
	using namespace VTX;
	using SelectionUtil = App::Test::Util::Selection;
	App::Fixture app;

	Test::loadSystem( "1AGA.mmtf" );
	Test::loadSystem( "4HHB.pdb" );
	Test::loadSystem( "8QHQ.pdb" );

	App::Component::Chemistry::System & mol4hhb
		= App::SCENE().getComponentByName<App::Component::Chemistry::System>( "4HHB" );

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_1",
			"select( system_names='4HHB' )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB" ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_indexes_1",
			"select( system_indexes=0 )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "1AGA" ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_chain_names_1",
			"select( system_names='4HHB', chain_names='A' )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_chain_indexes_1",
			"select( system_names='4HHB', chain_indexes=1 )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 1 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_chain_names_1",
			"select( chain_names='A' )",
			SelectionUtil::createSelection(
				{ *SelectionUtil::generateSystemData( "4HHB", { 0 } ),
				  *SelectionUtil::generateSystemData( "1AGA", { 0 } ),
				  *SelectionUtil::generateSystemData( "8QHQ", { 0 } ) }
			)
		)
	);

	std::unique_ptr<App::Selection::SelectionData> allHistidineOn4HHB = SelectionUtil::generateSystemData(
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

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_residue_names_1",
			"select( system_names='4HHB', residue_names='HIS' )",
			SelectionUtil::createSelection( allHistidineOn4HHB )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_residue_names_1",
			"select( system_names='4HHB', residue_names='H' )",
			SelectionUtil::createSelection( allHistidineOn4HHB )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_residue_names_1",
			"select( system_names='4HHB', residue_names='Histidine' )",
			SelectionUtil::createSelection( allHistidineOn4HHB )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_system_names_chain_res_1",
			"select( system_names='4HHB', chain_names='B', residue_indexes=4 )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", {}, { 144 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_mol_chain_res_atom_1",
			"select( system_names='4HHB', chain_names='A', residue_indexes=10, atom_types='C')",
			SelectionUtil::createSelection(
				SelectionUtil::generateSystemData( "4HHB", {}, {}, { 66, 67, 69, 70, 71 } )
			)
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_mol_chain_res_atom_2",
			"select( system_names='4HHB', chain_names='A', residue_indexes=10, atom_names='C')",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", {}, {}, { 67 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_addition_1",
			"select( system_names='4HHB', chain_names='A' ) + select( system_names='4HHB', chain_names='C')",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0, 2 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_substract_1",
			"select(system_names='4HHB', chain_names='A' ) - select( system_names='4HHB', chain_names='B')",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_intersect_1",
			"intersect(select( system_names='4HHB' ), select( chain_names='A' ))",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB", { 0 } ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_intersect_1",
			"intersect(select( system_names='4HHB' ), select( residue_names='HIS' ))",
			SelectionUtil::createSelection( allHistidineOn4HHB )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_exclusive_1",
			"exclusive( (select( system_names='4HHB' ) - select(  system_names='4HHB', residue_names='HIS' )), "
			"select( system_names='4HHB', "
			"residue_names='HIS' "
			") )",
			SelectionUtil::createSelection( SelectionUtil::generateSystemData( "4HHB" ) )
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_empty_1", "select( system_names='Nimportequoi', chain_indexes=1 )", App::Selection::Selection()
		)
	);

	CHECK(
		SelectionUtil::checkSelection(
			"test_empty_2",
			"select( system_names='4HHB', residue_names='his' )",
			SelectionUtil::createSelection( allHistidineOn4HHB )
		)
	);

	// CHECK( SelectionUtil::checkSelection(
	//	"test_system_names_chain_str_i_1", "select( system_names='4HHB', chain_indexes='1' )",
	// PythonFixture::Application::Selection::Selection() ) ); // NO => manage param as str

	// interpretor.runCommand( "select( system_names='4HHB', residue_indexes=range(0, 100) )" );
};

TEST_CASE( "VTX_PYTHON_BINDING - Script execution via interpretor", "[python][binding][script][method]" )
{
	using namespace VTX;
	App::Fixture app;

	const FilePath internalDataDir = Util::Filesystem::getExecutableDir() / "data";
	const FilePath scriptPath	   = internalDataDir / "script_test.py";

	std::future<AsyncJobResult> _ret;
	INTERPRETOR().runScript( scriptPath, _ret );
	_ret.wait();
	CHECK( _ret.get().success == true );

	const FilePath badScriptPath = internalDataDir / "bad_script_test.py";

	INTERPRETOR().runScript( badScriptPath, _ret );
	_ret.wait();
	CHECK( _ret.get().success == false );
}
TEST_CASE( "VTX_PYTHON_BINDING - Script execution via command", "[python][nothing]" )
{
	using namespace VTX;
	App::Fixture				app;
	std::future<AsyncJobResult> _future;
	INTERPRETOR().runCommand( "s = 1", _future );
	_future.wait();
	// std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
}
TEST_CASE( "VTX_PYTHON_BINDING - Script execution via command", "[python][binding][command][script]" )
{
	using namespace VTX;
	App::Fixture app;

	const FilePath internalDataDir = Util::Filesystem::getExecutableDir() / "data";
	const FilePath scriptPath	   = internalDataDir / "script_test.py";

	std::future<AsyncJobResult> _future;
	std::stringstream			ssCommandRun = std::stringstream();
	ssCommandRun << "runScript(" << scriptPath << " )";
	INTERPRETOR().runCommand( ssCommandRun.str(), _future );
	_future.wait();
	CHECK( _future.get().success == true );

	INTERPRETOR().runCommand( "runScript('bzzzz')", _future );
	_future.wait();
	CHECK( _future.get().success == false );
}
