#include "io/util/chemfiles.hpp"

namespace VTX::IO::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::Core::ChemDB::Bond::ORDER p_bondOrder )
	{
		switch ( p_bondOrder )
		{
		case VTX::Core::ChemDB::Bond::ORDER::SINGLE: return chemfiles::Bond::BondOrder::SINGLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::DOUBLE: return chemfiles::Bond::BondOrder::DOUBLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::TRIPLE: return chemfiles::Bond::BondOrder::TRIPLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::QUADRUPLE: return chemfiles::Bond::BondOrder::QUADRUPLE; break;
		case VTX::Core::ChemDB::Bond::ORDER::QUINTUPLET: return chemfiles::Bond::BondOrder::QUINTUPLET; break;
		case VTX::Core::ChemDB::Bond::ORDER::AROMATIC: return chemfiles::Bond::BondOrder::AROMATIC; break;

		case VTX::Core::ChemDB::Bond::ORDER::UNKNOWN:
		default: return chemfiles::Bond::BondOrder::UNKNOWN; break;
		}
	}

} // namespace VTX::IO::Util::Chemfiles
