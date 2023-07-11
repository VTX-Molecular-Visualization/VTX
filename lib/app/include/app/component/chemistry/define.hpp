#ifndef __VTX_APP_COMPONENT_CHEMISTRY_DEFINE__
#define __VTX_APP_COMPONENT_CHEMISTRY_DEFINE__

#include "_fwd.hpp"
#include <core/struct/atom.hpp>
#include <core/struct/bond.hpp>
#include <core/struct/chain.hpp>
#include <core/struct/molecule.hpp>
#include <core/struct/residue.hpp>

namespace VTX::App::Component::Chemistry
{
	using AtomCore	   = VTX::Core::Template::Atom<Molecule, Residue>;
	using BondCore	   = VTX::Core::Template::Bond<Molecule>;
	using ResidueCore  = VTX::Core::Template::Residue<Molecule, Chain>;
	using ChainCore	   = VTX::Core::Template::Chain<Molecule>;
	using MoleculeCore = VTX::Core::Template::Molecule<Chain, Residue, Atom, Bond>;
} // namespace VTX::App::Component::Chemistry

#endif
