#ifndef VTX_APP_PYTHON_BINDING_TOPOLOGY_BINDER_HPP
#define VTX_APP_PYTHON_BINDING_TOPOLOGY_BINDER_HPP

#include "app/python_binding/module.hpp"

namespace VTX::App::PythonBinding
{
	class TopologyBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
