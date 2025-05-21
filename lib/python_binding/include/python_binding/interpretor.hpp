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

		/**
		 * @brief Register a binder. This allow external code to bind python function and classes to c++ code.
		 * @param p_binder
		 */
		void add( Binder p_binder );

		void clearBinders();

		/**
		 * @brief Run the input command and return its result's repr() (i.e. string representation of the instance) or
		 * an empty string if there is no return value.
		 * @param p_line
		 * @return
		 */
		std::string runCommand( const std::string & p_line ) const;

		/**
		 * @brief Run the script located in the path in argument.
		 * @param p_path
		 */
		void runScript( const FilePath & p_path ) const;

		/**
		 * @brief Load python script from input path as a module and return it.
		 * @param p_path
		 * @return
		 */
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
