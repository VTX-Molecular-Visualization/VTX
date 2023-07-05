#ifndef __VTX_CORE_STRUCT_RESIDUE__
#define __VTX_CORE_STRUCT_RESIDUE__

#include "_fwd.hpp"
#include "core/template/residue.hpp"
#include "define.hpp"

namespace VTX::Core::Struct
{
	class Residue : public ResidueCore
	{
	  public:
		Residue();
		Residue( const size_t p_internalIndex );
		Residue( Chain * const p_chain );
		Residue( Chain * const p_chain, const size_t p_internalIndex );
	};
} // namespace VTX::Core::Struct
#endif
