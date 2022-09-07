#include "structural_alignment.hpp"
#include "model/molecule.hpp"
#include "rmsd.hpp"
#include "structural_alignment_method/ce_align.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include <string>

namespace VTX::Analysis
{
	StructuralAlignment::AlignmentParameters::AlignmentParameters() :
		AlignmentParameters( AlignmentMethodEnum ::Unknown ) {};

	StructuralAlignment::AlignmentParameters::AlignmentParameters( const AlignmentMethodEnum & p_method ) :
		method( p_method )
	{
	}
	StructuralAlignment::AlignmentMethod::AlignmentMethod() {}
	StructuralAlignment::AlignmentResult::AlignmentResult() {}

	StructuralAlignment::AlignmentParameters * StructuralAlignment::instantiateDefaultParameters(
		const AlignmentMethodEnum & p_methodEnum )
	{
		switch ( p_methodEnum )
		{
		case AlignmentMethodEnum::CEAlign: return new StructuralAlignmentMethod::CEAlign::CustomParameters(); break;

		default:
			VTX_ERROR( "Not implemented method" );
			return nullptr;
			break;
		}
	}

	void StructuralAlignment::computeAlignment( const Model::Molecule &				   p_staticMolecule,
												const std::vector<Model::Molecule *> & p_mobilesMolecules,
												const AlignmentParameters &			   p_parameters )
	{
		Tool::Chrono chrono = Tool::Chrono();

		AlignmentMethod * method;

		switch ( p_parameters.method )
		{
		case AlignmentMethodEnum::CEAlign: method = new StructuralAlignmentMethod::CEAlign(); break;

		default: VTX_ERROR( "Not Implemented" ); return;
		}

		try
		{
			for ( Model::Molecule * const mobileMolecule : p_mobilesMolecules )
			{
				Tool::Chrono chrono = Tool::Chrono();
				chrono.start();
				const AlignmentResult result = method->compute( p_staticMolecule, *mobileMolecule, p_parameters );
				chrono.stop();
				VTX_DEBUG( "Alignment computed in " + chrono.elapsedTimeStr() );

				const Math::Transform transform
					= Math::Transform( p_staticMolecule.getTransform().get() * result.transformationMatrix );

				mobileMolecule->applyTransform( transform );

				const float rmsd = result.rmsd == -1
									   ? float( Analysis::RMSD::computeRMSD( p_staticMolecule, *mobileMolecule, true ) )
									   : result.rmsd;

				VTX_DEBUG( "RMSD : " + std::to_string( rmsd ) );
			}
		}
		catch ( const std::exception & e )
		{
			delete method;
			throw e;
		}

		delete method;
	}

} // namespace VTX::Analysis