#include "python_binding/api/chain.hpp"
#include "python_binding/api/residue.hpp"
#include "python_binding/api/system.hpp"
//
#include "python_binding/api/atom.hpp"

namespace VTX::PythonBinding::API
{
	Chain Atom::getChain()
	{
		if ( _ptr )
			return _ptr->getChain();
		return {};
	}

	const Chain Atom::getChain() const
	{
		if ( _ptr )
			return _ptr->getChain();
		return Chain();
	}
	Residue Atom::getResidue()
	{
		if ( _ptr )
			return _ptr->getResidue();
		return Residue();
	}
	const Residue Atom::getResidue() const
	{
		if ( _ptr )
			return _ptr->getResidue();
		return Residue();
	}

	System Atom::getSystem()
	{
		if ( _ptr )
			return _ptr->getSystem();
		return System::System();
	}
	const System Atom::getSystem() const
	{
		if ( _ptr )
			return _ptr->getSystem();
		return System::System();
	}
} // namespace VTX::PythonBinding::API
