#ifndef __VTX_PYTHON_BINDING_PYBIND11_EXTRA_CONVERTOR__
#define __VTX_PYTHON_BINDING_PYBIND11_EXTRA_CONVERTOR__

#include "python_binding/wrapper/arg.hpp"

namespace VTX::PythonBinding::Wrapper
{
	inline pybind11::arg convertToPybind11Extra( const Wrapper::Arg & p_arg ) { return p_arg.toPybind11Extra(); }

	template<typename V>
	pybind11::arg_v convertToPybind11Extra( const Wrapper::VArg<V> & p_arg )
	{
		return p_arg.toPybind11Extra();
	}
}; // namespace VTX::PythonBinding::Wrapper
#endif
