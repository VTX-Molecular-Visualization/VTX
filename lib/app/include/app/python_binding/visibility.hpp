#ifndef __VTX_APP_PYTHONBINDING_VISIBILITY__
#define __VTX_APP_PYTHONBINDING_VISIBILITY__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class VisibilityBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
