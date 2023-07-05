#ifndef __VTX_CORE_STRUCT_ATOM__
#define __VTX_CORE_STRUCT_ATOM__

#include "_fwd.hpp"
#include "core/template/atom.hpp"
#include "define.hpp"

namespace VTX::Core::Struct
{
	class Atom : public Core::Template::Atom<Molecule, Residue>
	{
	  public:
		Atom();
		Atom( Residue * const p_residue );
		Atom( Molecule * const p_molecule );
		Atom( Molecule * const p_molecule, const size_t p_internalIndex );
		Atom( MoleculeCore * const p_molecule, const size_t p_internalIndex );
	};
} // namespace VTX::Core::Struct
#endif
