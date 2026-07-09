#ifndef __VTX_APP_PYTHONBINDING_REPRESENTATION__
#define __VTX_APP_PYTHONBINDING_REPRESENTATION__

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class RepresentationBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
