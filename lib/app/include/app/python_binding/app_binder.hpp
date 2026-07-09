#ifndef __VTX_APP_PYTHONBINDING_APP_BINDER__
#define __VTX_APP_PYTHONBINDING_APP_BINDER__

#include <python_binding/binder.hpp>

namespace VTX::App::PythonBinding
{
	class AppBinder
	{
	  public:
		void bind( VTX::PythonBinding::Wrapper::Module & p_vtxmodule );
	};
} // namespace VTX::App::PythonBinding

#endif // !__VTX_APP_PYTHONBINDING_APP_BINDER__
