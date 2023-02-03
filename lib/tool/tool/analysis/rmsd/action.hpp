#ifndef __VTX_TOOL_ANALYSIS_RMSD_ACTION__
#define __VTX_TOOL_ANALYSIS_RMSD_ACTION__

#include "action/base_action.hpp"
#include "core/rmsd.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include <vector>

namespace VTX::Tool::Analysis::RMSD::Action
{
	class ComputeRMSD : public VTX::Action::BaseAction
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
			RMSDTarget( const Model::Molecule * p_target, std::vector<const Model::Molecule *> p_others ) :
				moleculeData { p_target, p_others }
			{
			}
			RMSDTarget( const Model::Selection * const p_selection ) : selectionData( p_selection ) {}
			~RMSDTarget() {};

			const std::pair<const Model::Molecule *, std::vector<const Model::Molecule *>> moleculeData;
			const Model::Selection * const												   selectionData;
		};

	  public:
		explicit ComputeRMSD( const Model::Molecule * const			 p_target,
							  std::vector<const Model::Molecule *> & p_others,
							  const bool							 p_considerTransform = true ) :
			_mode( MODE::MOLECULE ),
			_target( p_target, p_others ), _considerTransform( p_considerTransform )
		{
		}
		explicit ComputeRMSD( const Model::Selection & p_selection, const bool p_considerTransform = true ) :
			_mode( MODE::SELECTION ), _target( &p_selection ), _considerTransform( p_considerTransform )
		{
		}

		virtual void execute() override
		{
			switch ( _mode )
			{
			case MODE::MOLECULE:
			{
				for ( const Model::Molecule * const molecule : _target.moleculeData.second )
				{
					RMSD::callRMSDComputation( _target.moleculeData.first, molecule, _considerTransform );
				}
			}
			break;

			case MODE::SELECTION:
			{
				RMSD::callRMSDComputation( *_target.selectionData, _considerTransform );
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
