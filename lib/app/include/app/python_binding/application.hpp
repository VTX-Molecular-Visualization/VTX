#ifndef __VTX_APP_PYTHONBINDING_APPLICATION__
#define __VTX_APP_PYTHONBINDING_APPLICATION__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class ApplicationBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
