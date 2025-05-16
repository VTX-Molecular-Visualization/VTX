#include <pybind11/pybind11.h>
//
#include "python_binding/api/arguments.hpp"

namespace VTX::PythonBinding::API
{

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
		else if ( pybind11::isinstance<pybind11::iterable>( p_param ) ) // If it's a set, add values in res
		{
			// pybind11::list strSet = p_param.cast<pybind11::list>();
			res.reserve( pybind11::len( p_param ) );

			for ( const auto & item : p_param )
			{
				const T2 str = item.cast<T2>();
				res.emplace_back( str );
			}
		}

		return res;
	}

	void PythonKwargs::Del::operator()( pybind11::kwargs * p ) noexcept { delete p; }

	PythonKwargs::PythonKwargs( const pybind11::kwargs & p_ ) : _ptr( new pybind11::kwargs( p_ ) ) {}

	size_t PythonKwargs::size() const { return pybind11::len( *_ptr ); }
	bool   PythonKwargs::contains( const char * p_ ) const { return _ptr->contains( p_ ); }
	void   PythonKwargs::get( const char * p_key, std::string & p_value ) const
	{
		if ( !_ptr->contains( p_key ) )
		{
			p_value.clear();
			return;
		}
		p_value = _ptr->operator[]( p_key ).cast<std::string>();
	}
	void PythonKwargs::get( const char * p_key, std::vector<std::string> & p_value ) const
	{
		if ( !_ptr->contains( p_key ) )
		{
			p_value.clear();
			return;
		}

		p_value = _getListFromParam<pybind11::str, std::string>( _ptr->operator[]( p_key ) );
	}

	void PythonKwargs::get( const char * p_key, std::vector<size_t> & p_value ) const
	{
		if ( !_ptr->contains( p_key ) )
		{
			p_value.clear();
			return;
		}

		p_value = _getListFromParam<pybind11::int_, size_t>( _ptr->operator[]( p_key ) );
	}
	void PythonKwargs::get( const char * p_key, std::vector<uint32_t> & p_value ) const
	{
		if ( !_ptr->contains( p_key ) )
		{
			p_value.clear();
			return;
		}

		p_value = _getListFromParam<pybind11::int_, uint32_t>( _ptr->operator[]( p_key ) );
	}
} // namespace VTX::PythonBinding::API
