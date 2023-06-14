#ifndef __VTX_TOOL_ANALYSIS_UTIL__
#define __VTX_TOOL_ANALYSIS_UTIL__

#include "rmsd/core/rmsd.hpp"
#include <app/old/application/selection/_fwd.hpp>
#include <app/old/component/chemistry/_fwd.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Tool::Analysis::Util
{
	void pickTargetAndComparersFromSelection( const App::Old::Application::Selection::SelectionModel &	   p_selection,
											  const App::Old::Component::Chemistry::Molecule *&		   p_target,
											  std::vector<App::Old::Component::Chemistry::Molecule *> & p_comparers );
	void pickTargetAndComparersFromSelection( const App::Old::Application::Selection::SelectionModel &			 p_selection,
											  const App::Old::Component::Chemistry::Molecule *&				 p_target,
											  std::vector<const App::Old::Component::Chemistry::Molecule *> & p_comparers );

	void getAtomPositions( const App::Old::Application::Selection::SelectionModel & p_selection,
						   const App::Old::Component::Chemistry::Molecule *	  p_target,
						   std::vector<Vec3f> &							  p_positions );

	std::string getRMSDLog( const RMSD::Core::RMSDData & p_data );

} // namespace VTX::Tool::Analysis::Util

#endif
