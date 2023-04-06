#include "bond.hpp"
#include "molecule.hpp"

namespace VTX
{
	namespace Model
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
	} // namespace Model
} // namespace VTX
