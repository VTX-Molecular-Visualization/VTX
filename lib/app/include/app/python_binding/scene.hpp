#ifndef __VTX_APP_PYTHONBINDING_SCENE__
#define __VTX_APP_PYTHONBINDING_SCENE__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class SceneBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
