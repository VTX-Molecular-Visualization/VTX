#ifndef __VTX_UTIL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_UTIL_SOLVENT_EXCLUDED_SURFACE__

#include "model/molecule.hpp"
#include <unordered_set>

namespace VTX::Util::SolventExcludedSurface
{
	using memory_size_type = size_t;

	const memory_size_type SMALL_SES_MEMORY_THRESHOLD = memory_size_type( 8000000000 ); // 8Go

	bool			 checkSESMemory( const std::unordered_set<Model::Molecule *> & p_molecules );
	bool			 checkSESMemory( const Model::Molecule & p_molecule );
	memory_size_type estimateSESMemory( const Model::Molecule & p_molecule );

} // namespace VTX::Util::SolventExcludedSurface

#endif
