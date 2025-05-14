#include <pybind11/pybind11.h>
//
#include "python_binding/api/arguments.hpp"

namespace VTX::PythonBinding::API
{
	void   PythonKwargs::Del::operator()( pybind11::kwargs * p ) noexcept { delete p; }
	size_t PythonKwargs::size() const { return size_t(); }
	bool   PythonKwargs::contains( const char * ) const { return false; }
	void   PythonKwargs::get( const char * p_key, std::string & p_value ) const {}
	void   PythonKwargs::get( const char * p_key, std::vector<std::string> & p_value ) const {}
	void   PythonKwargs::get( const char * p_key, std::vector<uint32_t> & p_value ) const {}
} // namespace VTX::PythonBinding::API
