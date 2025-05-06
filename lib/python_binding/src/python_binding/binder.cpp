#include "python_binding/binder.hpp"
#include <pybind11/embed.h>

namespace VTX::PythonBinding
{
	void importFullModule( const std::string & p_moduleName ) noexcept
	{
		const std::string command = "from " + p_moduleName + " import *";
		pybind11::exec( command );
	}
	void importObject( const std::string & p_moduleName, const std::string & p_objectName ) noexcept
	{
		const std::string command = "from " + p_moduleName + " import " + p_objectName;
		pybind11::exec( command );
	}
} // namespace VTX::PythonBinding
