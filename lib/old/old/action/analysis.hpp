#ifndef __VTX_ACTION_ANALYSIS__
#define __VTX_ACTION_ANALYSIS__

#include "action/base_action.hpp"
#include "analysis/rmsd.hpp"
#include "analysis/structural_alignment.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "tool/logger.hpp"
#include "util/analysis.hpp"
#include <cmath>
#include <string>
#include <vector>

namespace VTX::Action::Analysis
{
	class ComputeRMSD : public BaseAction
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
					VTX::Analysis::RMSD::callRMSDComputation(
						_target.moleculeData.first, molecule, _considerTransform );
				}
			}
			break;

			case MODE::SELECTION:
			{
				VTX::Analysis::RMSD::callRMSDComputation( *_target.selectionData, _considerTransform );
			}
			break;
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const MODE _mode;

		const union RMSDTarget _target;
		const bool			   _considerTransform;
	};

	class ComputeStructuralAlignment : public BaseAction
	{
	  public:
		explicit ComputeStructuralAlignment(
			const Model::Molecule * const										  p_staticMolecule,
			const std::vector<Model::Molecule *> &								  p_mobileMolecules,
			const VTX::Analysis::StructuralAlignment::AlignmentParameters * const p_parameters ) :
			_staticMolecule( p_staticMolecule ),
			_mobileMolecules( p_mobileMolecules ), _parameters( p_parameters )
		{
		}

		explicit ComputeStructuralAlignment( const Model::Selection & p_selection )
		{
			Util::Analysis::pickTargetAndComparersFromSelection( p_selection, _staticMolecule, _mobileMolecules );

			_parameters = VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
				VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );
		}

		virtual void execute() override
		{
			Util::Chrono chrono = Util::Chrono();
			chrono.start();
			try
			{
				VTX::Analysis::StructuralAlignment::computeAlignment( _staticMolecule, _mobileMolecules, *_parameters );
				VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}
			catch ( std::exception & e )
			{
				chrono.stop();
				throw e;
			}

			chrono.stop();
		}

	  private:
		const Model::Molecule *											_staticMolecule;
		std::vector<Model::Molecule *>									_mobileMolecules;
		const VTX::Analysis::StructuralAlignment::AlignmentParameters * _parameters;
	};

} // namespace VTX::Action::Analysis
#endif
