#ifndef __VTX_PYTHON_BINDING_BINDING_GLOBAL__
#define __VTX_PYTHON_BINDING_BINDING_GLOBAL__

#include <string>

namespace VTX::PythonBinding::Binding
{
	class Global
	{
	  public:
		static void openFile( const std::string & p_path );
		static void runScript( const std::string & p_path );
	};
} // namespace VTX::PythonBinding::Binding

#endif
