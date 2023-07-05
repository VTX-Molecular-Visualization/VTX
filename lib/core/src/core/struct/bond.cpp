#include "core/struct/bond.hpp"
#include "core/struct/molecule.hpp"

namespace VTX::Core::Struct
{
	Bond::Bond() : BondCore() {}
	Bond::Bond( MoleculeCore * const p_moleculePtr, const size_t p_internalIndex ) :
		BondCore( static_cast<Molecule * const>( p_moleculePtr ), p_internalIndex )
	{
	}
	Bond::Bond( Molecule * const p_moleculePtr ) : BondCore( p_moleculePtr ) {}
	Bond::Bond( Molecule * const p_moleculePtr, const size_t p_internalIndex ) :
		BondCore( p_moleculePtr, p_internalIndex )
	{
	}
} // namespace VTX::Core::Struct
