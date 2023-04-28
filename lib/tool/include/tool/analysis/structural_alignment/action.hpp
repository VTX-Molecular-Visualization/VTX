#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_ACTION__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_ACTION__

#include "tool/analysis/util.hpp"
#include "core/structural_alignment.hpp"
#include <app/core/action/base_action.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/application/selection/selection.hpp>

#include <vector>

namespace VTX::Tool::Analysis::StructuralAlignment::Action
{
	class ComputeStructuralAlignment : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ComputeStructuralAlignment(
			const App::Component::Chemistry::Molecule * const								 p_staticMolecule,
			const std::vector<App::Component::Chemistry::Molecule *> &						 p_mobileMolecules,
			const Core::StructuralAlignment::AlignmentParameters * const p_parameters ) :
			_staticMolecule( p_staticMolecule ),
			_mobileMolecules( p_mobileMolecules ), _parameters( p_parameters )
		{
		}

		explicit ComputeStructuralAlignment( const App::Application::Selection::SelectionModel & p_selection )
		{
			Util::Analysis::pickTargetAndComparersFromSelection( p_selection, _staticMolecule, _mobileMolecules );

			_parameters = Core::StructuralAlignment::instantiateDefaultParameters(
				Core::StructuralAlignment::AlignmentMethodEnum::CEAlign );
		}

		virtual void execute() override
		{
			Util::Chrono chrono = Util::Chrono();
			chrono.start();
			try
			{
				Core::StructuralAlignment::computeAlignment( _staticMolecule, _mobileMolecules, *_parameters );
			}
			catch ( std::exception & e )
			{
				chrono.stop();
				throw e;
			}

			chrono.stop();
		}

	  private:
		const App::Component::Chemistry::Molecule *								   _staticMolecule;
		std::vector<App::Component::Chemistry::Molecule *>						   _mobileMolecules;
		const Core::StructuralAlignment::AlignmentParameters * _parameters;
	};

} // namespace VTX::Tool::Analysis::StructuralAlignment::Action

#endif
