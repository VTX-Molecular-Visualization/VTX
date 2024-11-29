#ifndef __VTX_PYTHON_BINDING_BINDER__
#define __VTX_PYTHON_BINDING_BINDER__

#include "python_binding/vtx_python_module.hpp"
#include <string>

namespace VTX::PythonBinding
{
	class PyTXModule;
	class Binder;

	template<typename BinderType>
	concept BinderConcept = std::derived_from<BinderType, Binder>;

	class Binder
	{
	  public:
		Binder()		  = default;
		virtual ~Binder() = default;

		virtual void bind( PyTXModule & p_pytxModule ) = 0;
		virtual void importHeaders() {};

	  protected:
		void _importFullModule( const std::string & p_module );
		void _importObject( const std::string & p_module, const std::string & p_object );
	};
} // namespace VTX::PythonBinding

#endif
