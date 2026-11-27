#pragma once

#include <python_binding/binder.hpp>

namespace VTX::App::PythonBinding
{
	class AppBinder
	{
	  public:
		void bind( VTX::PythonBinding::Wrapper::Module & p_vtxmodule );
	};
} // namespace VTX::App::PythonBinding
