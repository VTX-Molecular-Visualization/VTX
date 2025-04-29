#include "python_binding/binding/vtx_module.hpp"
#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/log_redirection.hpp"
#include <memory>
#include <pybind11/pybind11.h>
#include <string>
#include <util/logger.hpp>

namespace VTX::PythonBinding
{

	PYBIND11_MODULE( vtx_python_api, m )
	{
		m.doc() = "VTX Python module."; // optional module docstring

		// Class to redirect Python prints
		pybind11::class_<LogRedirection>( m, "LogRedirection", pybind11::module_local() )
			.def_static( "write", &LogRedirection::write )
			.def_static( "flush", &LogRedirection::flush );

		// Core module : Contains some core functions which must be hidden for users
		pybind11::module_ vtxCoreModule = m.def_submodule( "Core", "VTX Python core functions" );
		vtxCoreModule.doc()				= "Contains some core functions which must be hidden for users.";

		// Command module : Contains all commands accessible to user via command line.
		pybind11::module_ vtxCommandModule = m.def_submodule( "Command", "VTX Python command interface" );
		vtxCommandModule.doc() = "Command module : Contains all commands accessible to user via command line.";

		pybind11::module_ vtxAPIModule = m.def_submodule( "API", "VTX API." );
		vtxAPIModule.doc()			   = "VTX API module."; // optional module docstring

		Binding::applyVtxBinding( vtxAPIModule );
	}
} // namespace VTX::PythonBinding
