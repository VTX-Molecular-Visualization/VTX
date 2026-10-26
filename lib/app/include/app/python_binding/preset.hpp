#ifndef VTX_APP_PYTHON_BINDING_PRESET_HPP
#define VTX_APP_PYTHON_BINDING_PRESET_HPP

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class PresetBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
}

#endif
