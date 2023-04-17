#include "tool/analysis/structural_alignment/core/struct/residue_center_of_mass_data_set.hpp"
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/residue.hpp>

namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct
{
	Vec3f ResidueCenterOfMassDataSet::getPositionInMolecule( const Model::Residue & p_residue )
	{
		const Vec3f	  absoluteResidueCenterOfMass = ResidueDataSet::get( p_residue );
		const Vec3f & offset
			= p_residue.getMoleculePtr()->getCurrentAtomPositionFrame()[ p_residue.getIndexFirstAtom() ];

		return absoluteResidueCenterOfMass + offset;
	}

	Vec3f ResidueCenterOfMassDataSet::generateResidueData( const Model::Residue & p_residue ) const
	{
		const Model::Molecule &						molecule = *( p_residue.getMoleculePtr() );
		const Model::Molecule::AtomPositionsFrame & atomPositions
			= molecule.getAtomPositionFrame( molecule.getFrame() );

		Vec3f res = VEC3F_ZERO;

		for ( uint i = p_residue.getIndexFirstAtom(); i < p_residue.getIndexFirstAtom() + p_residue.getAtomCount();
			  i++ )
		{
			res += atomPositions[ i ];
		}

		res /= p_residue.getAtomCount();

		const Vec3f & basePos = atomPositions[ p_residue.getIndexFirstAtom() ];

		return ( res - basePos );
	}
} // namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct
