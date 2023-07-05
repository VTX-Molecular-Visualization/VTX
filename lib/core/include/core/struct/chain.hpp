#ifndef __VTX_CORE_STRUCT_CHAIN__
#define __VTX_CORE_STRUCT_CHAIN__

#include "_fwd.hpp"
#include "core/template/chain.hpp"
#include "define.hpp"

namespace VTX::Core::Struct
{
	class Chain : public ChainCore
	{
	  public:
		Chain();
		Chain( MoleculeCore * const p_molecule, const size_t p_internalIndex );
		Chain( Molecule * const p_molecule, const size_t p_internalIndex );
	};
} // namespace VTX::Core::Struct
#endif
