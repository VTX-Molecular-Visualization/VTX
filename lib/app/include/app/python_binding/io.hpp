#ifndef __VTX_APP_PYTHONBINDING_IO__
#define __VTX_APP_PYTHONBINDING_IO__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class IOBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
