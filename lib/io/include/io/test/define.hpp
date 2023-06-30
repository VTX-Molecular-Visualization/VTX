#ifndef __VTX_IO_TEST_DEFINE__
#define __VTX_IO_TEST_DEFINE__

#include "_fwd.hpp"
#include <core/struct/atom.hpp>
#include <core/struct/bond.hpp>
#include <core/struct/chain.hpp>
#include <core/struct/molecule.hpp>
#include <core/struct/residue.hpp>

namespace VTX::IO::Test
{
	using AtomCore	   = Core::Struct::Atom<CustomMolecule, CustomResidue>;
	using BondCore	   = Core::Struct::Bond<CustomMolecule>;
	using ResidueCore  = Core::Struct::Residue<CustomMolecule, CustomChain>;
	using ChainCore	   = Core::Struct::Chain<CustomMolecule>;
	using MoleculeCore = Core::Struct::Molecule<CustomChain, CustomResidue, CustomAtom, CustomBond>;

	class CustomMolecule : public MoleculeCore
	{
	  public:
		CustomMolecule();
	};
	class CustomChain : public ChainCore
	{
	  public:
		CustomChain();
		CustomChain( MoleculeCore * const p_molecule, const size_t p_internalIndex );
		CustomChain( CustomMolecule * const p_molecule, const size_t p_internalIndex );
	};
	class CustomResidue : public ResidueCore
	{
	  public:
		CustomResidue();
		CustomResidue( const size_t p_internalIndex );
		CustomResidue( CustomChain * const p_chain );
		CustomResidue( CustomChain * const p_chain, const size_t p_internalIndex );
	};
	class CustomAtom : public AtomCore
	{
	  public:
		CustomAtom();
		CustomAtom( CustomResidue * const p_residue );
		CustomAtom( CustomMolecule * const p_molecule );
		CustomAtom( CustomMolecule * const p_molecule, const size_t p_internalIndex );
		CustomAtom( MoleculeCore * const p_molecule, const size_t p_internalIndex );
	};
	class CustomBond : public BondCore
	{
	  public:
		CustomBond();
		CustomBond( CustomMolecule * const p_moleculePtr );
		CustomBond( CustomMolecule * const p_moleculePtr, const size_t p_internalIndex );
		CustomBond( MoleculeCore * const p_moleculePtr, const size_t p_internalIndex );
	};
} // namespace VTX::IO::Test
#endif
