#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__

#include <app/component/chemistry/_fwd.hpp>
#include <app/core/event/base_event_receiver_vtx.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/model/label.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <string>
#include <utility>

// namespace VTX::Model
//{
//	class Atom;
//	class Molecule;
// } // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class DistanceToCycle :
		public Model::Label,
		public App::Core::Event::BaseEventReceiverVTX,
		public Generic::BaseAutoDelete
	{
		VTX_MODEL
		//
		//	  private:
		//		using MoleculeView = App::Core::View::CallbackView<App::Component::Chemistry::Molecule,
		// Model::Measurement::DistanceToCycle>;
		//
		//	  public:
		//		using AtomPair = std::pair<const App::Component::Chemistry::Atom &, const
		// App::Component::Chemistry::Atom
		//&>;
		//
		//	  public:
		//		void setAtoms( const App::Component::Chemistry::Atom & p_firstAtom, const
		// App::Component::Chemistry::Atom
		//& p_secondAtom ); 		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;
		//
		//		void _recomputeAABB( Math::AABB & p_aabb ) override;
		//
		//		const App::Component::Chemistry::Atom & getFirstAtom() const { return *_firstAtom; }
		//		const App::Component::Chemistry::Atom & getSecondAtom() const { return *_secondAtom; }
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
		//		void _setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
		//								const App::Component::Chemistry::Atom & p_secondAtom,
		//								const bool			p_notify = true );
		//
		//		void _performAutoName( const bool p_notify = true ) override;
		//
		//		void _invalidate();
		//
		//	  private:
		//		const App::Component::Chemistry::Atom * _firstAtom	= nullptr;
		//		const App::Component::Chemistry::Atom * _secondAtom = nullptr;
		//		float				_distance	= 0.f;
		//
		//		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();
		//
		//		void _computeDistance( const bool p_notify = true );
		//		void _instantiateViewsOnMolecules();
		//		void _cleanViews();
		//
		//		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;
		//
		//		void _onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule, const
		// App::Core::Event::VTXEvent * const p_event
		//);
	};

} // namespace VTX::Model::Measurement
#endif
