#include "define.hpp"

namespace VTX::IO::Test
{
	CustomMolecule::CustomMolecule() : MoleculeCore() {};
	CustomChain::CustomChain() : ChainCore() {}
	CustomChain::CustomChain( MoleculeCore * const p_molecule, const size_t p_internalIndex ) :
		CustomChain( static_cast<CustomMolecule * const>( p_molecule ), p_internalIndex ) {};
	CustomChain::CustomChain( CustomMolecule * const p_molecule, const size_t p_internalIndex ) :
		ChainCore( p_molecule, p_internalIndex )
	{
	}

	CustomResidue::CustomResidue() : ResidueCore() {}
	CustomResidue::CustomResidue( const size_t p_internalIndex ) : ResidueCore( p_internalIndex ) {};
	CustomResidue::CustomResidue( CustomChain * const p_chain ) : ResidueCore( p_chain ) {};
	CustomResidue::CustomResidue( CustomChain * const p_chain, const size_t p_internalIndex ) :
		ResidueCore( p_chain, p_internalIndex ) {};

	CustomAtom::CustomAtom() : AtomCore() {}
	CustomAtom::CustomAtom( CustomResidue * const p_residue ) : AtomCore( p_residue ) {};
	CustomAtom::CustomAtom( CustomMolecule * const p_molecule ) : AtomCore( p_molecule ) {};
	CustomAtom::CustomAtom( CustomMolecule * const p_molecule, const size_t p_internalIndex ) :
		AtomCore( p_molecule, p_internalIndex ) {};
	CustomAtom::CustomAtom( MoleculeCore * const p_molecule, const size_t p_internalIndex ) :
		AtomCore( static_cast<CustomMolecule * const>( p_molecule ), p_internalIndex ) {};

	CustomBond::CustomBond() : BondCore() {}
	CustomBond::CustomBond( MoleculeCore * const p_moleculePtr, const size_t p_internalIndex ) :
		BondCore( static_cast<CustomMolecule * const>( p_moleculePtr ), p_internalIndex )
	{
	}
	CustomBond::CustomBond( CustomMolecule * const p_moleculePtr ) : BondCore( p_moleculePtr ) {}
	CustomBond::CustomBond( CustomMolecule * const p_moleculePtr, const size_t p_internalIndex ) :
		BondCore( p_moleculePtr, p_internalIndex )
	{
	}
} // namespace VTX::IO::Test
