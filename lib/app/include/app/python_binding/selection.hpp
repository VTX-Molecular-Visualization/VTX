#ifndef __VTX_APP_PYTHONBINDING_SELECTION__
#define __VTX_APP_PYTHONBINDING_SELECTION__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class SelectionBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
