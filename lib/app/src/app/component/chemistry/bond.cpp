#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Component::Chemistry
{
	bool Bond::comparer( const Bond & p_lhs, const Bond & p_rhs )
	{
		const uint lhsMoleculeID = p_lhs.getMoleculePtr()->getId();
		const uint rhsMoleculeID = p_rhs.getMoleculePtr()->getId();

		if ( lhsMoleculeID == rhsMoleculeID )
		{
			if ( p_lhs._indexFirstAtom == p_rhs._indexFirstAtom )
				return p_lhs._indexSecondAtom < p_rhs._indexSecondAtom;
			else
				return p_lhs._indexFirstAtom < p_rhs._indexFirstAtom;
		}
		else
		{
			return lhsMoleculeID < rhsMoleculeID;
		}
	}
} // namespace VTX::App::Component::Chemistry
