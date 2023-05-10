#include "core/struct/bond.hpp"
#include "core/struct/molecule.hpp"

namespace VTX::Core::Struct
{
	bool Bond::comparer( const Bond & p_lhs, const Bond & p_rhs )
	{
		const Molecule * const lhsMoleculeID = p_lhs.getMoleculePtr();
		const Molecule * const rhsMoleculeID = p_rhs.getMoleculePtr();

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
} // namespace VTX::Core::Struct
