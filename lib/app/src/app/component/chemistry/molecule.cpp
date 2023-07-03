#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include <algorithm>

namespace VTX::App::Component::Chemistry
{
	void Molecule::initAtoms( const size_t p_atomCount )
	{
		_atoms.resize( p_atomCount, new Atom( this ) );
		std::generate( _atoms.begin(), _atoms.end(), [ this, n = 0 ]() mutable { return new Atom( this, n++ ); } );

		_atomColors.resize( p_atomCount );
		_atomRadii.resize( p_atomCount, 0.f );
		_atomIds.resize( p_atomCount, INVALID_MODEL_ID );

		std::generate( _atomIds.begin(), _atomIds.end(), [ n = 0 ]() mutable { return n++; } );

		_atomVisibilities.resize( p_atomCount, 1 );
		_atomSelections.resize( p_atomCount, 0 );
	}

	void Molecule::initBonds( const size_t p_bondCount )
	{
		_bonds.resize( p_bondCount, nullptr );
		std::generate( _bonds.begin(), _bonds.end(), [ this, n = 0 ]() mutable { return new Bond( this, n++ ); } );

		_bondAtomPairIds.resize( p_bondCount * 2 );
	}

	const Util::Math::AABB & Molecule::getAABB()
	{
		// TODO Create AABB Component
		//  if ( !_aabb.isValid() )
		//	_recomputeAABB();

		return _aabb;
	}

} // namespace VTX::App::Component::Chemistry
