#ifndef __VTX_PYTHON_BINDING_INTERPRETOR__
#define __VTX_PYTHON_BINDING_INTERPRETOR__

#include "python_binding/binder.hpp"
#include <memory>
#include <string>
#include <util/singleton.hpp>
#include <util/types.hpp>

namespace VTX::PythonBinding
{
	namespace Wrapper
	{
		class Module;
	}

	/**
	 * @brief Wrapper around the python interpretor. Meant to be a singleton.
	 */
	class Interpretor
	{
	  public:
		Interpretor();
		~Interpretor();

		void add( Binder p_binder );

		void clearBinders();

		void			runCommand( const std::string & p_line ) const;
		void			runScript( const FilePath & p_path ) const;
		Wrapper::Module loadModule( const FilePath & p_path ) const;

		const PyTXModule & getModule() const;
		PyTXModule &	   getModule();

		void print( const std::string & p_line ) const;

		void getPythonModule( pybind11::module_ ** );

	  private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

} // namespace VTX::PythonBinding

namespace VTX
{
	inline PythonBinding::Interpretor & INTERPRETOR() { return Util::Singleton<PythonBinding::Interpretor>::get(); }

} // namespace VTX

#endif
