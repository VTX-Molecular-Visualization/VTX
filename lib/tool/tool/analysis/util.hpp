#ifndef __VTX_TOOL_ANALYSIS_UTIL__
#define __VTX_TOOL_ANALYSIS_UTIL__

#include "rmsd/core/rmsd.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Selection;
} // namespace VTX::Model

namespace VTX::Tool::Analysis::Util
{
	void pickTargetAndComparersFromSelection( const Model::Selection &		   p_selection,
											  const Model::Molecule *&		   p_target,
											  std::vector<Model::Molecule *> & p_comparers );
	void pickTargetAndComparersFromSelection( const Model::Selection &				 p_selection,
											  const Model::Molecule *&				 p_target,
											  std::vector<const Model::Molecule *> & p_comparers );

	void getAtomPositions( const Model::Selection & p_selection,
						   const Model::Molecule *	p_target,
						   std::vector<Vec3f> &		p_positions );

	std::string getRMSDLog( const RMSD::Core::RMSDData & p_data );

} // namespace VTX::Tool::Analysis::Util

#endif
