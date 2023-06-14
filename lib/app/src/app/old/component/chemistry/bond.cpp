#include "app/old/component/chemistry/bond.hpp"
#include "app/old/component/chemistry/molecule.hpp"

namespace VTX::App::Old::Component::Chemistry
{
	bool Bond::comparer( const Bond & p_lhs, const Bond & p_rhs )
	{
		const uint lhsMoleculeID = p_lhs.getMoleculePtr()->getId();
		const uint rhsMoleculeID = p_rhs.getMoleculePtr()->getId();

		if ( lhsMoleculeID == rhsMoleculeID )
		{
			if ( p_lhs.getIndexFirstAtom() == p_rhs.getIndexFirstAtom() )
				return p_lhs.getIndexSecondAtom() < p_rhs.getIndexSecondAtom();
			else
				return p_lhs.getIndexFirstAtom() < p_rhs.getIndexFirstAtom();
		}
		else
		{
			return lhsMoleculeID < rhsMoleculeID;
		}
	}

	void Bond::setMoleculePtr( Molecule * const p_molecule )
	{
		_moleculePtr = p_molecule;
		_bondStruct->setMoleculePtr( &_moleculePtr->getMoleculeStruct() );
	}
} // namespace VTX::App::Old::Component::Chemistry
