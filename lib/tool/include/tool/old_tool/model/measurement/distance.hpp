#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE__

#include <app/component/chemistry/_fwd.hpp>
#include <app/core/event/base_event_receiver_vtx.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/component/object3d/label.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/id.hpp>
#include <string>
#include <utility>
#include <vector>

namespace VTX::Model::Measurement
{
	class Distance : public App::Component::Object3D::Label, public App::Core::Event::BaseEventReceiverVTX, public Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView
			= App::Core::View::CallbackView<App::Component::Chemistry::Molecule, Model::Measurement::Distance>;

	  public:
		using AtomPair = std::pair<const App::Component::Chemistry::Atom &, const App::Component::Chemistry::Atom &>;

	  public:
		void setAtoms( const App::Component::Chemistry::Atom & p_firstAtom,
					   const App::Component::Chemistry::Atom & p_secondAtom );
		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		void _recomputeAABB( App::Component::Object3D::Helper::AABB & p_aabb ) override;

		const App::Component::Chemistry::Atom & getFirstAtom() const { return *_atoms[ 0 ]; }
		const App::Component::Chemistry::Atom & getSecondAtom() const { return *_atoms[ 1 ]; }

		float getDistance() const { return _distance; }
		bool  isValid() const;

		void autoDelete() const override;

	  protected:
		Distance();
		Distance( const AtomPair & p_pair );

		~Distance();

		void _setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
								const App::Component::Chemistry::Atom & p_secondAtom,
								const bool								p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const App::Component::Chemistry::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const App::Component::Chemistry::Molecule * const p_atom ) const;

		void _invalidate();

	  private:
		std::vector<const App::Component::Chemistry::Atom *> _atoms
			= std::vector<const App::Component::Chemistry::Atom *>();
		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();

		float _distance = 0.f;

		void _computeDistance( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::App::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule,
								const VTX::App::Core::Event::VTXEvent * const	  p_event );
	};

} // namespace VTX::Model::Measurement
#endif
