#ifndef __VTX_CORE_STRUCT_BOND__
#define __VTX_CORE_STRUCT_BOND__

#include "_fwd.hpp"
#include "core/template/bond.hpp"
#include "define.hpp"

namespace VTX::Core::Struct
{
	class Bond : public BondCore
	{
	  public:
		Bond();
		Bond( MoleculeCore * const p_moleculePtr, const size_t p_internalIndex );
		Bond( Molecule * const p_moleculePtr );
		Bond( Molecule * const p_moleculePtr, const size_t p_internalIndex );
	};
} // namespace VTX::Core::Struct
#endif
