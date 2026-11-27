#pragma once

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class PresetBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
}
