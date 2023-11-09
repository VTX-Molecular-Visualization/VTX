#ifndef __VTX_PYTHON_BINDING_BINDING_VTX_APP_BINDER__
#define __VTX_PYTHON_BINDING_BINDING_VTX_APP_BINDER__

#include "python_binding/binder.hpp"
#include "python_binding/wrapper/module.hpp"

namespace VTX::PythonBinding::Binding
{
	class VTXAppBinder final : public Binder
	{
		void bind( PyTXModule & p_vtxmodule ) override;
	};
} // namespace VTX::PythonBinding::Binding

#endif
