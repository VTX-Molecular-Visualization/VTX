#ifndef __VTX_PYTHON_BINDING_BINDER__
#define __VTX_PYTHON_BINDING_BINDER__

#include "pytx_module.hpp"

namespace VTX::PythonBinding
{
	class Binder
	{
	  public:
		Binder()		  = default;
		virtual ~Binder() = default;

		virtual void bind( PyTXModule & p_pytxModule ) = 0;
	};
} // namespace VTX::PythonBinding

#endif
