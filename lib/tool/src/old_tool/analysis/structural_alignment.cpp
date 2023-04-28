#include "tool/old_tool/analysis/structural_alignment.hpp"
#include "tool/old_tool/analysis/rmsd.hpp"
#include "tool/old_tool/analysis/structural_alignment_method/ce_align.hpp"
#include <app/core/event/vtx_event.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/component/chemistry/molecule.hpp>
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
	StructuralAlignment::AlignmentResult::AlignmentResult( const App::Component::Chemistry::Molecule * const p_staticMolecule,
														   const App::Component::Chemistry::Molecule * const p_mobileMolecule ) :
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

	void StructuralAlignment::computeAlignment( const App::Component::Chemistry::Molecule * const		   p_staticMolecule,
												const std::vector<App::Component::Chemistry::Molecule *> & p_mobilesMolecules,
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
			for ( App::Component::Chemistry::Molecule * const mobileMolecule : p_mobilesMolecules )
			{
				Util::Chrono chrono = Util::Chrono();
				chrono.start();
				const AlignmentResult result = method->compute( *p_staticMolecule, *mobileMolecule, p_parameters );
				chrono.stop();
				VTX_INFO( "Alignment computed in " + chrono.elapsedTimeStr() );

				const App::Internal::Math::Transform transform
					= App::Internal::Math::Transform( p_staticMolecule->getTransform().get() * result.transformationMatrix );

				mobileMolecule->applyTransform( transform );

				VTX_EVENT<const AlignmentResult &>( VTX::App::Event::Global::STRUCTURAL_ALIGNMENT_COMPUTED, result );
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
