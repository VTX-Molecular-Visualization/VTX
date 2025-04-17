#include "python_binding/api/atom.hpp"
#include "python_binding/api/chain.hpp"
#include "python_binding/api/residue.hpp"

//
#include "python_binding/api/system.hpp"

namespace VTX::PythonBinding::API
{
	Atom System::getAtom( const atom_index_t p_index )
	{
		if ( _ptr )
			return _ptr->getAtom( p_index );
		return {};
	}
	const Atom System::getAtom( const atom_index_t p_index ) const
	{
		if ( _ptr )
			return _ptr->getAtom( p_index );
		return {};
	}
} // namespace VTX::PythonBinding::API
