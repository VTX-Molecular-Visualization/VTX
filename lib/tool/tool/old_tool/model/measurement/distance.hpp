#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE__

#include <app/old_app/event/base_event_receiver_vtx.hpp>
#include <app/old_app/event/event.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/model/label.hpp>
#include <app/old_app/view/callback_view.hpp>
#include <string>
#include <utility>
#include <vector>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class Distance : public Model::Label, Event::BaseEventReceiverVTX, Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::Distance>;

	  public:
		using AtomPair = std::pair<const Model::Atom &, const Model::Atom &>;

	  public:
		void setAtoms( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom );
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void _recomputeAABB( Object3D::Helper::AABB & p_aabb ) override;

		const Model::Atom & getFirstAtom() const { return *_atoms[ 0 ]; }
		const Model::Atom & getSecondAtom() const { return *_atoms[ 1 ]; }

		float getDistance() const { return _distance; }
		bool  isValid() const;

		void autoDelete() const override;

	  protected:
		Distance();
		Distance( const AtomPair & p_pair );

		~Distance();

		void _setAtomsInternal( const Model::Atom & p_firstAtom,
								const Model::Atom & p_secondAtom,
								const bool			p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const Model::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const Model::Molecule * const p_atom ) const;

		void _invalidate();

	  private:
		std::vector<const Model::Atom *> _atoms			= std::vector<const Model::Atom *>();
		std::vector<MoleculeView *>		 _moleculeViews = std::vector<MoleculeView *>();

		float _distance = 0.f;

		void _computeDistance( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event );
	};

} // namespace VTX::Model::Measurement
#endif
