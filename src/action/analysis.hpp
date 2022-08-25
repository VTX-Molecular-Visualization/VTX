#ifndef __VTX_ACTION_ANALYSIS__
#define __VTX_ACTION_ANALYSIS__

#include "action/base_action.hpp"
#include "analysis/rmsd.hpp"
#include "analysis/structural_alignment.hpp"
#include "analysis/structural_alignment_method/ce_align.hpp"
#include "analysis/structural_alignment_method/ce_align_pymol.hpp"
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
		explicit ComputeStructuralAlignment( const Model::Molecule * const			p_staticMolecule,
											 const std::vector<Model::Molecule *> & p_mobileMolecules ) :
			_staticMolecule( p_staticMolecule ),
			_mobileMolecules( p_mobileMolecules )
		{
		}

		virtual void execute() override
		{
			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			VTX::Analysis::StructuralAlignment::AlignmentParameters * const ceParameters
				= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
					VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );
			try
			{
				VTX::Analysis::StructuralAlignment::computeAlignment(
					*_staticMolecule, _mobileMolecules, *ceParameters );
			}
			catch ( std::exception & e )
			{
				delete ceParameters;
				chrono.stop();
				throw e;
			}

			chrono.stop();
			delete ceParameters;

			VTX_INFO( "elpased time : " + chrono.elapsedTimeStr() );
		}

	  private:
		const Model::Molecule * const		 _staticMolecule;
		const std::vector<Model::Molecule *> _mobileMolecules;
	};

	class ComputePymolStructuralAlignment : public BaseAction
	{
	  public:
		explicit ComputePymolStructuralAlignment( const Model::Molecule * const p_firstMolecule,
												  Model::Molecule * const		p_secondMolecule ) :
			_firstMolecule( p_firstMolecule ),
			_secondMolecule( p_secondMolecule )
		{
		}

		virtual void execute() override
		{
			VTX_INFO( "Compute Structural alignment" );

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			VTX::Analysis::StructuralAlignment::AlignmentParameters * ceParameters
				= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
					VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign_Pymol );
			try
			{
				const int minMolSize = _firstMolecule->getResidueCount() < _secondMolecule->getResidueCount()
										   ? _firstMolecule->getResidueCount()
										   : _secondMolecule->getResidueCount();

				ceParameters->windowSize = std::min( 8, int( sqrt( minMolSize ) ) );

				std::vector<Model::Molecule *> mobilesMolecules = std::vector<Model::Molecule *>();
				mobilesMolecules.emplace_back( _secondMolecule );

				VTX::Analysis::StructuralAlignment::computeAlignment(
					*_firstMolecule, mobilesMolecules, *ceParameters );
			}
			catch ( std::exception & e )
			{
				delete ceParameters;
				chrono.stop();
				throw e;
			}

			chrono.stop();
			delete ceParameters;

			VTX_INFO( "elpased time : " + chrono.elapsedTimeStr() );
		}

	  private:
		const Model::Molecule * const _firstMolecule;
		Model::Molecule * const		  _secondMolecule;
	};
} // namespace VTX::Action::Analysis
#endif
