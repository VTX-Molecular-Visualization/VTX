#ifndef __VTX_PYTHON_BINDING_BINDING_VTX__
#define __VTX_PYTHON_BINDING_BINDING_VTX__

#include "python_binding/binder.hpp"
#include "python_binding/wrapper/module.hpp"

namespace VTX::PythonBinding::Binding
{
	class VTXAppBinder : public Binder
	{
		void bind( Wrapper::Module & p_vtxmodule ) override;
	};
} // namespace VTX::PythonBinding::Binding

#endif
