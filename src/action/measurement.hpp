#ifndef __VTX_ACTION_MEASUREMENT__
#define __VTX_ACTION_MEASUREMENT__

#include "action/base_action.hpp"
#include "model/atom.hpp"
#include "model/measurement/distance.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Measurement
{
	class Distance : public BaseAction
	{
	  public:
		explicit Distance( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom ) :
			_firstAtom( p_firstAtom ), _secondAtom( p_secondAtom )
		{
		}

		virtual void execute() override
		{
			// TODO : perf test, to remove
			//			const Model::Molecule &				mol = *(
			// VTXApp::get().getScene().getMolecules().begin()->first
			//); 			std::default_random_engine			generator; 			std::uniform_int_distribution<uint>
			// distribution( 0, mol.getAtomCount() ); 			for ( int i = 0; i < 100; i++ )
			//			{
			//				const uint firstAtomIndex  = distribution( generator );
			//				const uint secondAtomIndex = distribution( generator );
			//
			//				Model::Measurement::Distance * const distanceModel
			//					= MVC::MvcManager::get().instantiateModel<Model::Measurement::Distance>(
			//						Model::Measurement::Distance::AtomPair( *mol.getAtom( firstAtomIndex ),
			//																*mol.getAtom( secondAtomIndex ) ) );
			//
			//				VTXApp::get().getScene().addLabel( distanceModel );
			//			}
			//
			//			return;

			Model::Measurement::Distance * const distanceModel
				= MVC::MvcManager::get().instantiateModel<Model::Measurement::Distance>(
					Model::Measurement::Distance::AtomPair( _firstAtom, _secondAtom ) );

			VTXApp::get().getScene().addLabel( distanceModel );
		}

	  private:
		const Model::Atom & _firstAtom;
		const Model::Atom & _secondAtom;
	};

} // namespace VTX::Action::Measurement
#endif
