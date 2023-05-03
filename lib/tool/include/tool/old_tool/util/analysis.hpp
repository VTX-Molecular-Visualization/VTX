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
	void pickTargetAndComparersFromSelection( const VTX::App::Application::Selection::SelectionModel &	p_selection,
											  const VTX::App::Component::Chemistry::Molecule *&			p_target,
											  std::vector<VTX::App::Component::Chemistry::Molecule *> & p_comparers );
	void pickTargetAndComparersFromSelection(
		const VTX::App::Application::Selection::SelectionModel &		p_selection,
		const VTX::App::Component::Chemistry::Molecule *&				p_target,
		std::vector<const VTX::App::Component::Chemistry::Molecule *> & p_comparers );

	void getAtomPositions( const VTX::App::Application::Selection::SelectionModel & p_selection,
						   const VTX::App::Component::Chemistry::Molecule *			p_target,
						   std::vector<Vec3f> &										p_positions );

	std::string getRMSDLog( const VTX::Analysis::RMSD::RMSDData & p_data );
} // namespace VTX::Util::Analysis

#endif
