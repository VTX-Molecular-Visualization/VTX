#ifndef __VTX_UTIL_ANALYSIS__
#define __VTX_UTIL_ANALYSIS__

#include "tool/old_tool/analysis/rmsd.hpp"
#include <app/application/selection/_fwd.hpp>
#include <app/component/chemistry/_fwd.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Util::Analysis
{
	void pickTargetAndComparersFromSelection( const App::Application::Selection::SelectionModel &	   p_selection,
											  const App::Component::Chemistry::Molecule *&		   p_target,
											  std::vector<App::Component::Chemistry::Molecule *> & p_comparers );
	void pickTargetAndComparersFromSelection( const App::Application::Selection::SelectionModel &			 p_selection,
											  const App::Component::Chemistry::Molecule *&				 p_target,
											  std::vector<const App::Component::Chemistry::Molecule *> & p_comparers );

	void getAtomPositions( const App::Application::Selection::SelectionModel & p_selection,
						   const App::Component::Chemistry::Molecule *	  p_target,
						   std::vector<Vec3f> &							  p_positions );

	std::string getRMSDLog( const VTX::Analysis::RMSD::RMSDData & p_data );
} // namespace VTX::Util::Analysis

#endif
