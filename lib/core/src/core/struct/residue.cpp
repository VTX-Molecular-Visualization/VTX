#include "core/struct/residue.hpp"
#include "core/struct/chain.hpp"

namespace VTX::Core::Struct
{
	Residue::Residue() : ResidueCore() {}
	Residue::Residue( const size_t p_internalIndex ) : ResidueCore( p_internalIndex ) {};
	Residue::Residue( Chain * const p_chain ) : ResidueCore( p_chain ) {};
	Residue::Residue( Chain * const p_chain, const size_t p_internalIndex ) :
		ResidueCore( p_chain, p_internalIndex ) {};
} // namespace VTX::Core::Struct
