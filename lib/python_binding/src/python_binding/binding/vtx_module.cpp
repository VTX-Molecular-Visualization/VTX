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
		m.doc() = "VTX Python module."; // optional module docstring

		// Global pointer to VTX data
		// pybind11::class_<PythonFixture::Core::System::SystemHandler,
		// std::shared_ptr<PythonFixture::Core::System::SystemHandler>>(
		//	m, "VTXSystem"
		//);

		// Class to redirect Python prints
		pybind11::class_<LogRedirection>( m, "LogRedirection" )
			// pybind11::class_<LogRedirection, std::shared_ptr<LogRedirection>>( m, "LogRedirection" )
			.def( "write", &LogRedirection::write )
			.def( "flush", &LogRedirection::flush );

		// Core module : Contains some core functions which must be hidden for users
		pybind11::module_ vtxCoreModule = m.def_submodule( "Core", "VTX Python core functions" );
		vtxCoreModule.doc()				= "Contains some core functions which must be hidden for users.";

		// vtxCoreModule.def( "_init", &_init, "Initialize python environment for commands" );

		// Command module : Contains all commands accessible to user via command line.
		pybind11::module_ vtxCommandModule = m.def_submodule( "Command", "VTX Python command interface" );
		vtxCommandModule.doc() = "Command module : Contains all commands accessible to user via command line.";

		pybind11::module_ vtxAPIModule = m.def_submodule( "API", "VTX API." );
		apiModule( vtxAPIModule );
	}
} // namespace VTX::PythonBinding
