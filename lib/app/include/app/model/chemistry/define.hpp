#ifndef __VTX_APP_MODEL_CHEMISTRY_DEFINE__
#define __VTX_APP_MODEL_CHEMISTRY_DEFINE__

#include "_fwd.hpp"
#include <core/new_struct/atom.hpp>
#include <core/new_struct/bond.hpp>
#include <core/new_struct/chain.hpp>
#include <core/new_struct/molecule.hpp>
#include <core/new_struct/residue.hpp>

namespace VTX::App::Model::Chemistry
{
	using AtomCore	   = VTX::Core::NewStruct::Atom<Molecule, Residue>;
	using BondCore	   = VTX::Core::NewStruct::Bond<Molecule>;
	using ResidueCore  = VTX::Core::NewStruct::Residue<Molecule, Chain>;
	using ChainCore	   = VTX::Core::NewStruct::Chain<Molecule>;
	using MoleculeCore = VTX::Core::NewStruct::Molecule<Chain, Residue, Atom, Bond>;
} // namespace VTX::App::Model::Chemistry

#endif
