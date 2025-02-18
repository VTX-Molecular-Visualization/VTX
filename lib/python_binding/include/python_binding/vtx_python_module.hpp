#ifndef __VTX_PYTHON_BINDING_PYTX_MODULE__
#define __VTX_PYTHON_BINDING_PYTX_MODULE__

#include "python_binding/wrapper/module.hpp"

namespace VTX::PythonBinding
{
	/**
	 * @brief Class responsible for holding VTX python modules declared within the binary
	 */
	class PyTXModule final
	{
	  public:
		PyTXModule( Wrapper::Module  p_mainModule );

		const Wrapper::Module & core() const;
		Wrapper::Module &		core();

		const Wrapper::Module & commands() const;
		Wrapper::Module &		commands();

	  private:
		Wrapper::Module _mainModule;
		Wrapper::Module _coreModule;
		Wrapper::Module _commandModule;
	};
}; // namespace VTX::PythonBinding
#endif
