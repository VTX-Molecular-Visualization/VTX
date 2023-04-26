#ifndef __VTX_MODEL_MEASUREMENT_ANGLE__
#define __VTX_MODEL_MEASUREMENT_ANGLE__

#include <app/component/chemistry/_fwd.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/core/event/base_event_receiver_vtx.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/model/label.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace VTX::Model::Measurement
{
	class Angle : public Model::Label, public App::Core::Event::BaseEventReceiverVTX, public Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView
			= App::Core::View::CallbackView<App::Component::Chemistry::Molecule, Model::Measurement::Angle>;

	  public:
		using AtomTriplet = std::tuple<const App::Component::Chemistry::Atom &,
									   const App::Component::Chemistry::Atom &,
									   const App::Component::Chemistry::Atom &>;

	  public:
		void setAtoms( const App::Component::Chemistry::Atom & p_firstAtom,
					   const App::Component::Chemistry::Atom & p_secondAtom,
					   const App::Component::Chemistry::Atom & p_thirdAtom );

		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		void _recomputeAABB( Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const App::Component::Chemistry::Atom *> getAtoms() const { return _atoms; };
		const App::Component::Chemistry::Atom &					   getFirstAtom() const { return *_atoms[ 0 ]; }
		const App::Component::Chemistry::Atom &					   getSecondAtom() const { return *_atoms[ 1 ]; }
		const App::Component::Chemistry::Atom &					   getThirdAtom() const { return *_atoms[ 2 ]; }

		float getAngle() const { return _angle; }
		bool  isValid() const;

		void autoDelete() const override;

		void _invalidate();

	  protected:
		Angle();
		Angle( const AtomTriplet & p_triplet );

		~Angle();

		void _setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
								const App::Component::Chemistry::Atom & p_secondAtom,
								const App::Component::Chemistry::Atom & p_thirdAtom,
								const bool								p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const App::Component::Chemistry::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const App::Component::Chemistry::Molecule * const p_atom ) const;

	  private:
		std::vector<const App::Component::Chemistry::Atom *> _atoms
			= std::vector<const App::Component::Chemistry::Atom *>();
		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();

		float _angle					= 0.f;
		bool  _isAllAtomsOnSameMolecule = false;

		void _computeAngle( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule,
								const VTX::App::Core::Event::VTXEvent * const	  p_event );
	};

} // namespace VTX::Model::Measurement
#endif
