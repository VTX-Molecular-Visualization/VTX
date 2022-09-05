#ifndef __VTX_ACTION_ANALYSIS__
#define __VTX_ACTION_ANALYSIS__

#include "action/base_action.hpp"
#include "analysis/rmsd.hpp"
#include "analysis/structural_alignment.hpp"
#include "model/molecule.hpp"
#include "tool/logger.hpp"
#include <cmath>
#include <string>
#include <vector>

namespace VTX::Action::Analysis
{
	class ComputeRMSD : public BaseAction
	{
	  public:
		explicit ComputeRMSD( const Model::Molecule * const			 p_target,
							  std::vector<const Model::Molecule *> & p_others,
							  const bool							 p_considerTransform = true ) :
			_target( *p_target ),
			_others( p_others ), _considerTransform( p_considerTransform )
		{
		}

		virtual void execute() override
		{
			for ( const Model::Molecule * const molecule : _others )
			{
				const float rmsdResult = VTX::Analysis::RMSD::computeRMSD( _target, *molecule, _considerTransform );
				VTX_INFO( "RMSD between " + _target.getDisplayName() + " and " + molecule->getDisplayName() + ": "
						  + std::to_string( rmsdResult ) );
			}
		}

	  private:
		const Model::Molecule &					   _target;
		const std::vector<const Model::Molecule *> _others;

		const bool _considerTransform;
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

		virtual void execute() override
		{
			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			try
			{
				VTX::Analysis::StructuralAlignment::computeAlignment(
					*_staticMolecule, _mobileMolecules, *_parameters );
			}
			catch ( std::exception & e )
			{
				chrono.stop();
				throw e;
			}

			chrono.stop();

			VTX_INFO( "elpased time : " + chrono.elapsedTimeStr() );
		}

	  private:
		const Model::Molecule * const										  _staticMolecule;
		const std::vector<Model::Molecule *>								  _mobileMolecules;
		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const _parameters;
	};
} // namespace VTX::Action::Analysis
#endif
