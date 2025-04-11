#include "python_binding/api/selection/selection_interpretor.hpp"
#include "python_binding/api/selection/selection_wrapper.hpp"

namespace VTX::PythonBinding::API::Selection
{
#ifdef JEVEUPAS

	void SelectionInterpretor::addInterpretor( const InterpretArgFunc & p_interpretor )
	{
		_selectionInterpretors.emplace_back( p_interpretor );
	}
	void SelectionInterpretor::clear() { _selectionInterpretors.clear(); }

	// Function that manage a param that can be a single str or a list of str and convert it into a vector of str
	SelectionWrapper SelectionInterpretor::select( const pybind11::kwargs & kwargs )
	{
		SelectionWrapper res = SelectionWrapper();

		if ( pybind11::len( kwargs ) == 0 )
			return res;

		for ( const InterpretArgFunc & p_func : _selectionInterpretors )
			p_func( *res._selection, kwargs );

		if ( kwargs.contains( "name" ) )
		{
			res.save( pybind11::str( kwargs[ "name" ] ) );
		}

		return res;
	};
#endif // JEVEUPAS

} // namespace VTX::PythonBinding::API::Selection
