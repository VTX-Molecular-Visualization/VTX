#ifndef __VTX_APP_PYTHON_BINDING_PRESET__
#define __VTX_APP_PYTHON_BINDING_PRESET__

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
