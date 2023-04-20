#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE_TO_CYCLE__

#include <app/event/base_event_receiver_vtx.hpp>
#include <app/event/vtx_event.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/model/label.hpp>
#include <app/view/callback_view.hpp>
#include <string>
#include <utility>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class DistanceToCycle : public Model::Label, Event::BaseEventReceiverVTX, Generic::BaseAutoDelete
	{
		VTX_MODEL
		//
		//	  private:
		//		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::DistanceToCycle>;
		//
		//	  public:
		//		using AtomPair = std::pair<const Model::Atom &, const Model::Atom &>;
		//
		//	  public:
		//		void setAtoms( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom );
		//		void receiveEvent( const Event::VTXEvent & p_event ) override;
		//
		//		void _recomputeAABB( Math::AABB & p_aabb ) override;
		//
		//		const Model::Atom & getFirstAtom() const { return *_firstAtom; }
		//		const Model::Atom & getSecondAtom() const { return *_secondAtom; }
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
		//		void _setAtomsInternal( const Model::Atom & p_firstAtom,
		//								const Model::Atom & p_secondAtom,
		//								const bool			p_notify = true );
		//
		//		void _performAutoName( const bool p_notify = true ) override;
		//
		//		void _invalidate();
		//
		//	  private:
		//		const Model::Atom * _firstAtom	= nullptr;
		//		const Model::Atom * _secondAtom = nullptr;
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
		//		void _onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event
		//);
	};

} // namespace VTX::Model::Measurement
#endif
