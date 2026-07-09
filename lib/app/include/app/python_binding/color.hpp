#ifndef __VTX_APP_PYTHONBINDING_COLOR__
#define __VTX_APP_PYTHONBINDING_COLOR__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class ColorBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
