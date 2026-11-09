#ifndef VTX_APP_PYTHON_BINDING_SCENE_HPP
#define VTX_APP_PYTHON_BINDING_SCENE_HPP

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
