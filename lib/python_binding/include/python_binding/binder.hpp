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

	/**
	 * @brief Class responsible for associating actions with python commands
	 */
	class Binder
	{
	  public:
		Binder()		  = default;
		virtual ~Binder() = default;

		/**
		 * @brief Method to reimplement to bind actions to commands.
		 * @param p_pytxModule Module provided by the interpretor to bind actions into.
		 */
		virtual void bind( PyTXModule & p_pytxModule ) = 0;

		/**
		 * @brief Children class can optionally re-implement this method to add module import instructions (e.g. when
		 * dealing with package dependancies )
		 */
		virtual void importHeaders() {};

	  protected:
		void _importFullModule( const std::string & p_module );
		void _importObject( const std::string & p_module, const std::string & p_object );
	};
} // namespace VTX::PythonBinding

#endif
