#include "core/struct/chain.hpp"
#include "core/struct/molecule.hpp"

namespace VTX::Core::Struct
{
	Chain::Chain() : ChainCore() {}
	Chain::Chain( MoleculeCore * const p_molecule, const size_t p_internalIndex ) :
		Chain( static_cast<Molecule * const>( p_molecule ), p_internalIndex ) {};
	Chain::Chain( Molecule * const p_molecule, const size_t p_internalIndex ) : ChainCore( p_molecule, p_internalIndex )
	{
	}
} // namespace VTX::Core::Struct
