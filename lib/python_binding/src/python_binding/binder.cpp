#include "python_binding/binder.hpp"
#include <pybind11/embed.h>

namespace VTX::PythonBinding
{
	void Binder::_importFullModule( const std::string & p_module )
	{
		const std::string command = "from " + p_module + " import *";
		pybind11::exec( command );
	}
	void Binder::_importObject( const std::string & p_module, const std::string & p_object )
	{
		const std::string command = "from " + p_module + " import " + p_object;
		pybind11::exec( command );
	}
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
