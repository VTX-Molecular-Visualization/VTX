#ifndef __VTX_APP_PYTHONBINDING_CAMERA__
#define __VTX_APP_PYTHONBINDING_CAMERA__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class CameraBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
