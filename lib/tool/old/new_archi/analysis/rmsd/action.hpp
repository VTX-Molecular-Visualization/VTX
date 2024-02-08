#ifndef __VTX_TOOL_ANALYSIS_RMSD_ACTION__
#define __VTX_TOOL_ANALYSIS_RMSD_ACTION__

#include "core/rmsd.hpp"
#include <app/old/action.hpp>
#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/application/selection/selection.hpp>
#include <vector>

namespace VTX::Tool::Analysis::RMSD::Action
{
	class ComputeRMSD : public VTX::App::Old::Core::Action::BaseAction
	{
	  private:
		enum class MODE : int
		{
			MOLECULE,
			SELECTION
		};

		union RMSDTarget
		{
			RMSDTarget() {};
			RMSDTarget( const App::Old::Component::Chemistry::Molecule * p_target, std::vector<const App::Old::Component::Chemistry::Molecule *> p_others ) :
				moleculeData { p_target, p_others }
			{
			}
			RMSDTarget( const App::Old::Application::Selection::SelectionModel * const p_selection ) : selectionData( p_selection ) {}
			~RMSDTarget() {};

			const std::pair<const App::Old::Component::Chemistry::Molecule *, std::vector<const App::Old::Component::Chemistry::Molecule *>> moleculeData;
			const App::Old::Application::Selection::SelectionModel * const												   selectionData;
		};

	  public:
		explicit ComputeRMSD( const App::Old::Component::Chemistry::Molecule * const			 p_target,
							  std::vector<const App::Old::Component::Chemistry::Molecule *> & p_others,
							  const bool							 p_considerTransform = true ) :
			_mode( MODE::MOLECULE ),
			_target( p_target, p_others ), _considerTransform( p_considerTransform )
		{
		}
		explicit ComputeRMSD( const App::Old::Application::Selection::SelectionModel & p_selection, const bool p_considerTransform = true ) :
			_mode( MODE::SELECTION ), _target( &p_selection ), _considerTransform( p_considerTransform )
		{
		}

		virtual void execute() override
		{
			switch ( _mode )
			{
			case MODE::MOLECULE:
			{
				for ( const App::Old::Component::Chemistry::Molecule * const molecule : _target.moleculeData.second )
				{
					RMSD::Core::callRMSDComputation( _target.moleculeData.first, molecule, _considerTransform );
				}
			}
			break;

			case MODE::SELECTION:
			{
				RMSD::Core::callRMSDComputation( *_target.selectionData, _considerTransform );
			}
			break;
			}
		}

	  private:
		const MODE _mode;

		const union RMSDTarget _target;
		const bool			   _considerTransform;
	};

} // namespace VTX::Tool::Analysis::RMSD::Action

#endif