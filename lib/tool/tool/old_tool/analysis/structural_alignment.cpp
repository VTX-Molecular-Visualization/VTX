#include "structural_alignment.hpp"
#include "rmsd.hpp"
#include "structural_alignment_method/ce_align.hpp"
#include <app/old_app/event/event.hpp>
#include <app/old_app/event/event_manager.hpp>
#include <app/old_app/model/molecule.hpp>
#include <string>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Analysis
{
	StructuralAlignment::AlignmentParameters::AlignmentParameters() :
		AlignmentParameters( AlignmentMethodEnum ::Unknown ) {};

	StructuralAlignment::AlignmentParameters::AlignmentParameters( const AlignmentMethodEnum & p_method ) :
		method( p_method )
	{
	}
	StructuralAlignment::AlignmentMethod::AlignmentMethod() {}
	StructuralAlignment::AlignmentResult::AlignmentResult( const Model::Molecule * const p_staticMolecule,
														   const Model::Molecule * const p_mobileMolecule ) :
		staticMolecule( p_staticMolecule ),
		mobileMolecule( p_mobileMolecule )
	{
	}

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

	void StructuralAlignment::computeAlignment( const Model::Molecule * const		   p_staticMolecule,
												const std::vector<Model::Molecule *> & p_mobilesMolecules,
												const AlignmentParameters &			   p_parameters )
	{
		Util::Chrono chrono = Util::Chrono();

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
				Util::Chrono chrono = Util::Chrono();
				chrono.start();
				const AlignmentResult result = method->compute( *p_staticMolecule, *mobileMolecule, p_parameters );
				chrono.stop();
				VTX_INFO( "Alignment computed in " + chrono.elapsedTimeStr() );

				const Math::Transform transform
					= Math::Transform( p_staticMolecule->getTransform().get() * result.transformationMatrix );

				mobileMolecule->applyTransform( transform );

				VTX_EVENT( new Event::VTXEventRef<const AlignmentResult>( Event::Global::STRUCTURAL_ALIGNMENT_COMPUTED,
																		  result ) );
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
