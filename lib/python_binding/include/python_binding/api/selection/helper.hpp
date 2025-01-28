#ifndef __VTX_PYTHON_API_SELECTION_HELPER__
#define __VTX_PYTHON_API_SELECTION_HELPER__

#include <app/selection/selection.hpp>
#include <functional>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <string>
#include <util/concepts.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::PythonBinding::API::Selection
{
	using SelectionObj = App::Selection::Selection;

	// Function that manage a param that can be a single T1 or a list of T1 and convert it into a vector of str
	template<typename T1, typename T2>
	static std::vector<T2> _getListFromParam( const pybind11::detail::item_accessor & p_param )
	{
		std::vector<T2> res = std::vector<T2>();

		if ( p_param.is_none() )
			return res;

		// If param is a single string, return a vector of size 1 with this string
		if ( pybind11::isinstance<T1>( p_param ) )
		{
			const T2 singleStr = p_param.cast<T2>();
			res.emplace_back( singleStr );
		}
		else if ( pybind11::isinstance<pybind11::set>( p_param ) ) // If it's a set, add values in res
		{
			pybind11::set strSet = p_param.cast<pybind11::set>();
			res.reserve( pybind11::len( strSet ) );

			for ( const auto & item : strSet )
			{
				const T2 str = item.cast<T2>();
				res.emplace_back( str );
			}
		}

		return res;
	}

	std::vector<std::string>  _getStringListInKwargs( const pybind11::kwargs & p_kwargs, const std::string & p_param );
	std::vector<size_t>		  _getIndexListInKwargs( const pybind11::kwargs & p_kwargs, const std::string & p_param );
	std::vector<atom_index_t> _getAtomIndexListInKwargs(
		const pybind11::kwargs & p_kwargs,
		const std::string &		 p_param
	);

	template<EnumConcept EnumType>
	std::vector<EnumType> _getEnumListFromStrInKwargs(
		const pybind11::kwargs &							   p_kwargs,
		const std::string &									   p_param,
		const std::function<EnumType( const std::string & )> & p_fromStrFunc,
		const std::optional<EnumType> &						   p_unknownEnum
	)
	{
		std::vector<EnumType> res;

		const std::vector<std::string> strList = _getStringListInKwargs( p_kwargs, p_param );

		res.reserve( strList.size() );

		for ( const std::string & p_str : strList )
		{
			const EnumType enumValue = p_fromStrFunc( p_str );

			if ( !( p_unknownEnum.has_value() && enumValue == p_unknownEnum ) )
				res.emplace_back( enumValue );
		}

		res.shrink_to_fit();

		return res;
	}

} // namespace VTX::PythonBinding::API::Selection
#endif
