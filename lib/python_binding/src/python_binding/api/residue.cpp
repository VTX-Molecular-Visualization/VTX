#include "python_binding/api/atom.hpp"
#include "python_binding/api/chain.hpp"
#include "python_binding/api/system.hpp"

//
#include "python_binding/api/residue.hpp"

namespace VTX::PythonBinding::API
{
	const Chain Residue::getChain() const
	{
		if ( _ptr )
			return _ptr->getChain();
		return Chain();
	}
	Chain Residue::getChain()
	{
		if ( _ptr )
			return _ptr->getChain();
		return Chain();
	}
	const System Residue::getSystem() const
	{
		if ( _ptr )
			return _ptr->getSystem();
		return System();
	}
	System Residue::getSystem()
	{
		if ( _ptr )
			return _ptr->getSystem();
		return System();
	}
} // namespace VTX::PythonBinding::API
