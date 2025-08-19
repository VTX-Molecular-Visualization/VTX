#include "python_binding/api/atom.hpp"
#include "python_binding/api/chain.hpp"
#include "python_binding/api/residue.hpp"

//
#include "python_binding/api/system.hpp"

namespace VTX::PythonBinding::API
{
	Chain System::getChain( const Index p_index )
	{
		if ( _ptr )
			return _ptr->getChain( p_index );
		return {};
	}
	const Chain System::getChain( const Index p_index ) const
	{
		if ( _ptr )
			return _ptr->getChain( p_index );
		return {};
	}
	Residue System::getResidue( const Index p_index )
	{
		if ( _ptr )
			return _ptr->getResidue( p_index );
		return {};
	}
	const Residue System::getResidue( const Index p_index ) const
	{
		if ( _ptr )
			return _ptr->getResidue( p_index );
		return {};
	}
	Atom System::getAtom( const Index p_index )
	{
		if ( _ptr )
			return _ptr->getAtom( p_index );
		return {};
	}
	const Atom System::getAtom( const Index p_index ) const
	{
		if ( _ptr )
			return _ptr->getAtom( p_index );
		return {};
	}
} // namespace VTX::PythonBinding::API
