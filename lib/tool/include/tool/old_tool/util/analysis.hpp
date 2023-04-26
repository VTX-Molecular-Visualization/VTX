#ifndef __VTX_UTIL_ANALYSIS__
#define __VTX_UTIL_ANALYSIS__

#include "tool/old_tool/analysis/rmsd.hpp"
#include <app/component/chemistry/_fwd.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Selection;
	} // namespace Model

	namespace Util::Analysis
	{
		void pickTargetAndComparersFromSelection( const Model::Selection &							   p_selection,
												  const App::Component::Chemistry::Molecule *&		   p_target,
												  std::vector<App::Component::Chemistry::Molecule *> & p_comparers );
		void pickTargetAndComparersFromSelection(
			const Model::Selection &								   p_selection,
			const App::Component::Chemistry::Molecule *&			   p_target,
			std::vector<const App::Component::Chemistry::Molecule *> & p_comparers );

		void getAtomPositions( const Model::Selection &					   p_selection,
							   const App::Component::Chemistry::Molecule * p_target,
							   std::vector<Vec3f> &						   p_positions );

		std::string getRMSDLog( const VTX::Analysis::RMSD::RMSDData & p_data );
	} // namespace Util::Analysis

} // namespace VTX

#endif
