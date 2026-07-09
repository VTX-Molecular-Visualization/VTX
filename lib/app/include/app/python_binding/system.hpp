#ifndef __VTX_APP_PYTHONBINDING_SYSTEM__
#define __VTX_APP_PYTHONBINDING_SYSTEM__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class SystemBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
