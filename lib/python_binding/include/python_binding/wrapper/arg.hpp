#ifndef __VTX_PYTHON_BINDING_WRAPPER_ARG__
#define __VTX_PYTHON_BINDING_WRAPPER_ARG__

#include <pybind11/pybind11.h>
#include <string>

namespace VTX::PythonBinding::Wrapper
{
	/**
	 * @brief Class allowing to declare positional arguments for a python callable
	 */
	class Arg
	{
	  public:
		Arg( const std::string & p_parameterName ) : _parameterName( p_parameterName ) {};
		virtual ~Arg() {}

		pybind11::arg toPybind11Extra() const { return pybind11::arg( _parameterName.c_str() ); }

	  protected:
		std::string _parameterName;
	};

	/**
	 * @brief Class allowing to declare named arguments for a python callable
	 * @tparam T
	 */
	template<typename T>
	class VArg : public Arg
	{
	  public:
		VArg( const std::string & p_parameterName, const T && p_defaultValue ) :
			Arg( p_parameterName ), _value( p_defaultValue )
		{
		}
		pybind11::arg_v toPybind11Extra() const { return pybind11::arg_v( _parameterName.c_str(), _value ); }

	  private:
		T _value;
	};

}; // namespace VTX::PythonBinding::Wrapper
#endif
