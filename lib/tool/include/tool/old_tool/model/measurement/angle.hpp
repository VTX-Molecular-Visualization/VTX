#ifndef __VTX_MODEL_MEASUREMENT_ANGLE__
#define __VTX_MODEL_MEASUREMENT_ANGLE__

#include <app/old/component/chemistry/_fwd.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/generic/base_auto_delete.hpp>
#include <app/old/component/object3d/label.hpp>
#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/core/view/callback_view.hpp>
#include <app/old/id.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace VTX::Model::Measurement
{
	class Angle :
		public App::Old::Component::Object3D::Label,
		public App::Old::Core::Event::BaseEventReceiverVTX,
		public App::Old::Component::Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView
			= App::Old::Core::View::CallbackView<App::Old::Component::Chemistry::Molecule, Model::Measurement::Angle>;

	  public:
		using AtomTriplet = std::tuple<const App::Old::Component::Chemistry::Atom &,
									   const App::Old::Component::Chemistry::Atom &,
									   const App::Old::Component::Chemistry::Atom &>;

	  public:
		void setAtoms( const App::Old::Component::Chemistry::Atom & p_firstAtom,
					   const App::Old::Component::Chemistry::Atom & p_secondAtom,
					   const App::Old::Component::Chemistry::Atom & p_thirdAtom );

		void receiveEvent( const App::Old::Core::Event::VTXEvent & p_event ) override;

		void _recomputeAABB( App::Old::Component::Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const App::Old::Component::Chemistry::Atom *> getAtoms() const { return _atoms; };
		const App::Old::Component::Chemistry::Atom &					   getFirstAtom() const { return *_atoms[ 0 ]; }
		const App::Old::Component::Chemistry::Atom &					   getSecondAtom() const { return *_atoms[ 1 ]; }
		const App::Old::Component::Chemistry::Atom &					   getThirdAtom() const { return *_atoms[ 2 ]; }

		float getAngle() const { return _angle; }
		bool  isValid() const;

		void autoDelete() const override;

		void _invalidate();

	  protected:
		Angle();
		Angle( const AtomTriplet & p_triplet );

		~Angle();

		void _setAtomsInternal( const App::Old::Component::Chemistry::Atom & p_firstAtom,
								const App::Old::Component::Chemistry::Atom & p_secondAtom,
								const App::Old::Component::Chemistry::Atom & p_thirdAtom,
								const bool								p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const App::Old::Component::Chemistry::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const App::Old::Component::Chemistry::Molecule * const p_atom ) const;

	  private:
		std::vector<const App::Old::Component::Chemistry::Atom *> _atoms
			= std::vector<const App::Old::Component::Chemistry::Atom *>();
		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();

		float _angle					= 0.f;
		bool  _isAllAtomsOnSameMolecule = false;

		void _computeAngle( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::App::Old::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const App::Old::Component::Chemistry::Molecule * const p_molecule,
								const VTX::App::Old::Core::Event::VTXEvent * const	  p_event );
	};

} // namespace VTX::Model::Measurement
#endif
