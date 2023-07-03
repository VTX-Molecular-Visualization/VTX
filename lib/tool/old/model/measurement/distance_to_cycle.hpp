#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__

#include <app/old/component/chemistry/_fwd.hpp>
#include <app/old/component/generic/base_auto_delete.hpp>
#include <app/old/component/object3d/label.hpp>
#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/core/view/callback_view.hpp>
#include <app/old/id.hpp>
#include <string>
#include <utility>

namespace VTX::Model::Measurement
{
	class DistanceToCycle :
		public App::Old::Component::Object3D::Label,
		public App::Old::Core::Event::BaseEventReceiverVTX,
		public App::Old::Component::Generic::BaseAutoDelete
	{
		VTX_MODEL
		//
		//	  private:
		//		using MoleculeView = App::Old::Core::View::CallbackView<App::Old::Component::Chemistry::Molecule,
		// Model::Measurement::DistanceToCycle>;
		//
		//	  public:
		//		using AtomPair = std::pair<const App::Old::Component::Chemistry::Atom &, const
		// App::Old::Component::Chemistry::Atom
		//&>;
		//
		//	  public:
		//		void setAtoms( const App::Old::Component::Chemistry::Atom & p_firstAtom, const
		// App::Old::Component::Chemistry::Atom
		//& p_secondAtom ); 		void receiveEvent( const App::Old::Core::Event::VTXEvent & p_event ) override;
		//
		//		void _recomputeAABB( Math::AABB & p_aabb ) override;
		//
		//		const App::Old::Component::Chemistry::Atom & getFirstAtom() const { return *_firstAtom; }
		//		const App::Old::Component::Chemistry::Atom & getSecondAtom() const { return *_secondAtom; }
		//
		//		float getDistance() const { return _distance; }
		//		bool  isValid() { return _firstAtom != nullptr && _secondAtom != nullptr; }
		//
		//		void autoDelete() const override;
		//
		//	  protected:
		//		DistanceToCycle();
		//		DistanceToCycle( const AtomPair & p_pair );
		//
		//		~DistanceToCycle();
		//
		//		void _setAtomsInternal( const App::Old::Component::Chemistry::Atom & p_firstAtom,
		//								const App::Old::Component::Chemistry::Atom & p_secondAtom,
		//								const bool			p_notify = true );
		//
		//		void _performAutoName( const bool p_notify = true ) override;
		//
		//		void _invalidate();
		//
		//	  private:
		//		const App::Old::Component::Chemistry::Atom * _firstAtom	= nullptr;
		//		const App::Old::Component::Chemistry::Atom * _secondAtom = nullptr;
		//		float				_distance	= 0.f;
		//
		//		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();
		//
		//		void _computeDistance( const bool p_notify = true );
		//		void _instantiateViewsOnMolecules();
		//		void _cleanViews();
		//
		//		VTX::App::Old::VTX_ID getViewID( const int p_atomPos ) const;
		//
		//		void _onMoleculeChange( const App::Old::Component::Chemistry::Molecule * const p_molecule, const
		// App::Old::Core::Event::VTXEvent * const p_event
		//);
	};

} // namespace VTX::Model::Measurement
#endif
