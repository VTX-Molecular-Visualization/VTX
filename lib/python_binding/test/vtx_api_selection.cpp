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

	interpretor.runCommand( "select( mol_n='4HHB' )" );					   // OK
	interpretor.runCommand( "select( mol_i=0 )" );						   // NOT OK => Need to manage indexes in scene
	interpretor.runCommand( "select( mol_n='4HHB', chain_n='A' )" );	   // OK
	interpretor.runCommand( "select( mol_n='4HHB', res_n='HIS' )" );	   // OK
	interpretor.runCommand( "select( mol_n='4HHB', res_n='H' )" );		   // OK
	interpretor.runCommand( "select( mol_n='4HHB', res_n='Histidine' )" ); // OK
	interpretor.runCommand( "select( mol_n='4HHB', chain_n='A', res_i=5)" ); // NO
	interpretor.runCommand( "select( mol_n='4HHB', res_i=5)" );	   // NO => Original index in scene not managed
	interpretor.runCommand( "select( mol_n='4HHB', atom_t='C')" ); // OK
	interpretor.runCommand( "select( mol_n='4HHB', res_n='HIS', atom_n='O')" ); // OK

	// interpretor.runCommand( "print(select( mol_n='4HHB', chain_n='A' ) + select( mol_n='4HHB', chain_n='B'))" );

	// interpretor.runCommand( "select( mol_n='Nimportequoi', chain_i='1' )" ); // OK
	//  interpretor.runCommand( "select( mol_n='4HHB', chain_i='1' )" ); // OK
	// interpretor.runCommand( "select( mol_n='4HHB', res_n='his' )" ); // NO => manage case
	// interpretor.runCommand( "select( mol_n='4HHB', res_i=range(0, 100) )" );
};
