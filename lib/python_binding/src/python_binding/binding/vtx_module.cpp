#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/log_redirection.hpp"
#include <app/vtx_app.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <string>
#include <util/logger.hpp>

namespace VTX
{
	// void _init( std::shared_ptr<PythonFixture::Core::System::SystemHandler> p_system )
	//{
	//  APP::referenceSystemHandler( p_system );
	//}
} // namespace VTX

namespace VTX::PythonBinding
{
	PYBIND11_MODULE( vtx_python_bin, m )
	{
		// return;							// A2E1
		m.doc() = "VTX Python module."; // optional module docstring
		// return;							// A2E2

		// Global pointer to VTX data
		// pybind11::class_<PythonFixture::Core::System::SystemHandler,
		// std::shared_ptr<PythonFixture::Core::System::SystemHandler>>(
		//	m, "VTXSystem"
		//);
		// Class to redirect Python prints
		// pybind11::class_<LogRedirection>( m, "LogRedirection" )
		pybind11::class_<LogRedirection, std::shared_ptr<LogRedirection>>( m, "LogRedirection" ) // TODO :
																								 // shared_ptr useful ?
			.def( "write", &LogRedirection::write )
			.def( "flush", &LogRedirection::flush );
		// return; // A2E3

		// Core module : Contains some core functions which must be hidden for users
		pybind11::module_ vtxCoreModule = m.def_submodule( "Core", "VTX Python core functions" );
		// return; // A2E4
		vtxCoreModule.doc() = "Contains some core functions which must be hidden for users.";
		// return; // A2E5

		// vtxCoreModule.def( "_init", &_init, "Initialize python environment for commands" );

		// Command module : Contains all commands accessible to user via command line.
		pybind11::module_ vtxCommandModule = m.def_submodule( "Command", "VTX Python command interface" );
		vtxCommandModule.doc() = "Command module : Contains all commands accessible to user via command line.";
		// return; // A2E6

		// pybind11::module_ vtxAPIModule = m.def_submodule( "API", "VTX API." );
		//  return; // A2E7
		//  apiModule( vtxAPIModule );
		//  return; // A2E8
	}
} // namespace VTX::PythonBinding
