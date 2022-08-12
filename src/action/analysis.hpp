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

namespace VTX::Action::Analysis
{
	class ComputeRMSD : public BaseAction
	{
	  public:
		explicit ComputeRMSD( const Model::Molecule & p_firstMolecule,
							  const Model::Molecule & p_secondMolecule,
							  const bool			  p_considerTransform = true ) :
			_firstMolecule( p_firstMolecule ),
			_secondMolecule( p_secondMolecule ), _considerTransform( p_considerTransform )
		{
		}

		virtual void execute() override
		{
			const float rmsdResult
				= VTX::Analysis::RMSD::computeRMSD( _firstMolecule, _secondMolecule, _considerTransform );
			VTX_INFO( "RMSD : " + std::to_string( rmsdResult ) );
		}

	  private:
		const Model::Molecule & _firstMolecule;
		const Model::Molecule & _secondMolecule;

		const bool _considerTransform;
	};

	class ComputeStructuralAlignment : public BaseAction
	{
	  public:
		explicit ComputeStructuralAlignment( const Model::Molecule * const p_firstMolecule,
											 Model::Molecule * const	   p_secondMolecule ) :
			_firstMolecule( p_firstMolecule ),
			_secondMolecule( p_secondMolecule )
		{
		}

		virtual void execute() override
		{
			VTX_INFO( "Compute Structural alignment" );

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			VTX::Analysis::StructuralAlignment::AlignmentParameters * const ceParameters
				= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
					VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );

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
