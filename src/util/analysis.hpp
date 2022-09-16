#ifndef __VTX_UTIL_ANALYSIS__
#define __VTX_UTIL_ANALYSIS__

#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Selection;
	} // namespace Model

	namespace Util::Analysis
	{
		void pickTargetAndComparersFromSelection( const Model::Selection &		   p_selection,
												  const Model::Molecule *&		   p_target,
												  std::vector<Model::Molecule *> & p_comparers );
		void pickTargetAndComparersFromSelection( const Model::Selection &				 p_selection,
												  const Model::Molecule *&				 p_target,
												  std::vector<const Model::Molecule *> & p_comparers );
	} // namespace Util::Analysis

} // namespace VTX

#endif
