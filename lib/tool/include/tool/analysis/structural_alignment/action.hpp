#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_ACTION__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_ACTION__

#include "tool/analysis/util.hpp"
#include "core/structural_alignment.hpp"
#include <app/old_app/action/base_action.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/selection.hpp>

#include <vector>

namespace VTX::Tool::Analysis::StructuralAlignment::Action
{
	class ComputeStructuralAlignment : public VTX::Action::BaseAction
	{
	  public:
		explicit ComputeStructuralAlignment(
			const Model::Molecule * const								 p_staticMolecule,
			const std::vector<Model::Molecule *> &						 p_mobileMolecules,
			const Core::StructuralAlignment::AlignmentParameters * const p_parameters ) :
			_staticMolecule( p_staticMolecule ),
			_mobileMolecules( p_mobileMolecules ), _parameters( p_parameters )
		{
		}

		explicit ComputeStructuralAlignment( const Model::Selection & p_selection )
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
		const Model::Molecule *								   _staticMolecule;
		std::vector<Model::Molecule *>						   _mobileMolecules;
		const Core::StructuralAlignment::AlignmentParameters * _parameters;
	};

} // namespace VTX::Tool::Analysis::StructuralAlignment::Action

#endif
