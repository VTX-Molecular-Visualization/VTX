#ifndef VTX_APP_PYTHON_BINDING_APP_BINDER_HPP
#define VTX_APP_PYTHON_BINDING_APP_BINDER_HPP

#include <python_binding/binder.hpp>

namespace VTX::App::PythonBinding
{
	class AppBinder
	{
	  public:
		void bind( VTX::PythonBinding::Wrapper::Module & p_vtxmodule );
	};
} // namespace VTX::App::PythonBinding

#endif // !VTX_APP_PYTHON_BINDING_APP_BINDER_HPP
