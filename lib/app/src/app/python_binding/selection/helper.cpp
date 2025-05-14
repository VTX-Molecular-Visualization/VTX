#include "app/python_binding/selection/helper.hpp"

namespace VTX::App::PythonBinding::API::Selection
{
	std::vector<std::string> _getStringListInKwargs( const pybind11::kwargs & p_kwargs, const std::string & p_param )
	{
		std::vector<std::string> res;

		if ( !p_kwargs.contains( p_param ) )
			res = std::vector<std::string>();
		else
			res = _getListFromParam<pybind11::str, std::string>( p_kwargs[ p_param.c_str() ] );

		return res;
	}
	std::vector<size_t> _getIndexListInKwargs( const pybind11::kwargs & p_kwargs, const std::string & p_param )
	{
		std::vector<size_t> res;

		if ( !p_kwargs.contains( p_param ) )
			res = std::vector<size_t>();
		else
			res = _getListFromParam<pybind11::int_, size_t>( p_kwargs[ p_param.c_str() ] );

		return res;
	}
	std::vector<atom_index_t> _getAtomIndexListInKwargs(
		const pybind11::kwargs & p_kwargs,
		const std::string &		 p_param
	)
	{
		std::vector<atom_index_t> res;

		if ( !p_kwargs.contains( p_param ) )
			res = std::vector<atom_index_t>();
		else
			res = _getListFromParam<pybind11::int_, atom_index_t>( p_kwargs[ p_param.c_str() ] );

		return res;
	}
} // namespace VTX::App::PythonBinding::API::Selection
