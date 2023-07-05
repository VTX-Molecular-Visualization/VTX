#ifndef __VTX_CORE_STRUCT_DEFINE__
#define __VTX_CORE_STRUCT_DEFINE__

#include "_fwd.hpp"
#include <core/template/atom.hpp>
#include <core/template/bond.hpp>
#include <core/template/chain.hpp>
#include <core/template/molecule.hpp>
#include <core/template/residue.hpp>

namespace VTX::Core::Struct
{
	using AtomCore	   = Template::Atom<Molecule, Residue>;
	using BondCore	   = Template::Bond<Molecule>;
	using ResidueCore  = Template::Residue<Molecule, Chain>;
	using ChainCore	   = Template::Chain<Molecule>;
	using MoleculeCore = Template::Molecule<Chain, Residue, Atom, Bond>;

} // namespace VTX::Core::Struct
#endif
