#include "app/python_binding/selection/selection_interpretor.hpp"
#include "app/python_binding/selection/selection_wrapper.hpp"

namespace VTX::App::PythonBinding::Selection
{

	void SelectionInterpretor::addInterpretor( const InterpretArgFunc & p_interpretor )
	{
		_selectionInterpretors.emplace_back( p_interpretor );
	}
	void SelectionInterpretor::clear() { _selectionInterpretors.clear(); }

	// Function that manage a param that can be a single str or a list of str and convert it into a vector of str
	SelectionWrapper SelectionInterpretor::select( const PythonKwargs & kwargs )
	{
		SelectionWrapper res = SelectionWrapper();

		if ( kwargs.size() == 0 )
			return res;

		for ( const InterpretArgFunc & p_func : _selectionInterpretors )
			p_func( *res._selection, kwargs );

		if ( kwargs.contains( "name" ) )
		{
			std::string value;
			kwargs.get( "name", value );
			res.save( value );
		}

		return res;
	};

} // namespace VTX::App::PythonBinding::Selection
