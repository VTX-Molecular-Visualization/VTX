#include "util/solvent_excluded_surface.hpp"
#include "model/solvent_excluded_surface.hpp"
#include "tool/logger.hpp"

namespace VTX::Util::SolventExcludedSurface
{
	bool checkSESMemory( const std::unordered_set<Model::Molecule *> & p_molecules )
	{
		memory_size_type memorySum = 0;

		for ( const Model::Molecule * const mol : p_molecules )
		{
			if ( !mol->hasSolventExcludedSurface() )
				memorySum += estimateSESMemory( *mol );
		}

		return memorySum > SMALL_SES_MEMORY_THRESHOLD;
	}
	bool checkSESMemory( const Model::Molecule & p_molecule )
	{
		if ( p_molecule.hasSolventExcludedSurface() )
			return false;

		return estimateSESMemory( p_molecule ) > SMALL_SES_MEMORY_THRESHOLD;
	}

	memory_size_type estimateSESMemory( const Model::Molecule & p_molecule ) { return SMALL_SES_MEMORY_THRESHOLD - 1; }
} // namespace VTX::Util::SolventExcludedSurface
