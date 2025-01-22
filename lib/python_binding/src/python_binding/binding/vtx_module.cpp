#include "python_binding/binding/vtx_module.hpp"
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
	const char * vtx_module_name() { return "vtx_python_bin"; }

	const char * vtx_initialization_script_name() { return "vtx_module_init.py"; }

} // namespace VTX::PythonBinding
