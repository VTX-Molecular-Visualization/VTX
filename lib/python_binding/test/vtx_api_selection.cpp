#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - VTX API Selection Tests", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	std::unique_ptr<PythonBinding::Interpretor> interpretorPtr = App::Test::Util::App::createInterpretor();
	PythonBinding::Interpretor &				interpretor	   = *interpretorPtr;

	interpretor.init();

	App::Application::Scene & scene = App::VTXApp::get().getScene();

	App::Test::Util::App::loadMolecule( "1AGA.mmtf" );
	App::Test::Util::App::loadMolecule( "4HHB.pdb" );

	// interpretor.runCommand( "select( mol_n='4HHB' )" ); //OK
	// interpretor.runCommand( "select( mol_i=0 )" ); //NOT OK => Need to manage indexes in scene
	// interpretor.runCommand( "select( mol_n='4HHB', chain_n='A' )" ); //OK
	interpretor.runCommand( "select( mol_n='4HHB', chain_i='1' )" );
	// interpretor.runCommand( "select( mol_n='4HHB', res_n='HIS' )" );
	// interpretor.runCommand( "select( mol_n='4HHB', res_i=5)" );
	// interpretor.runCommand( "select( mol_n='4HHB', atom_n='C')" );
	// interpretor.runCommand( "select( mol_n='4HHB', res_n='HIS', atom_n='O')" );

	// interpretor.runCommand( "select( mol_n='4HHB', res_i=range(0, 100) )" );
};
