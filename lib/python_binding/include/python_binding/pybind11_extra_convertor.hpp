#ifndef __VTX_PYTHON_BINDING_WRAPPER_PYBIND11_EXTRA_CONVERTOR__
#define __VTX_PYTHON_BINDING_WRAPPER_PYBIND11_EXTRA_CONVERTOR__

#include "wrapper/arg.hpp"

namespace VTX::PythonBinding
{
	class Pybind11ExtraConvertor
	{
	  public:
		static pybind11::arg convertToPybind11Extra( const Wrapper::Arg & p_arg )
		{
			pybind11::arg res = p_arg.toPybind11Extra();
			return res;
		};

		template<typename V>
		static pybind11::arg_v convertToPybind11Extra( const Wrapper::VArg<V> & p_arg )
		{
			pybind11::arg_v res = p_arg.toPybind11Extra();
			return res;
		};
	};

}; // namespace VTX::PythonBinding
#endif
