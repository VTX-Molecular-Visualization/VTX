#ifndef __VTX_APP_PYTHONBINDING_PYTHONBINDING__
#define __VTX_APP_PYTHONBINDING_PYTHONBINDING__

#include <python_binding/binder.hpp>

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Class responsible for binding vanilla actions to python commands
	 */
	class VTXAppBinder
	{
	  public:
		void bind( VTX::PythonBinding::PyTXModule & p_vtxmodule );
		void importHeaders();
	};
} // namespace VTX::App::PythonBinding

#endif // !__VTX_APP_PYTHONBINDING_PYTHONBINDING__
