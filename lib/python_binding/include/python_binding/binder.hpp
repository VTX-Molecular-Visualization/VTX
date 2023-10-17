#ifndef __VTX_PYTHON_BINDING_BINDER__
#define __VTX_PYTHON_BINDING_BINDER__

#include "wrapper/module.hpp"

namespace VTX::PythonBinding
{
	class Binder
	{
	  public:
		Binder()		  = default;
		virtual ~Binder() = default;

		virtual void bind( Wrapper::Module & p_vtxModule ) = 0;
	};
} // namespace VTX::PythonBinding

#endif
