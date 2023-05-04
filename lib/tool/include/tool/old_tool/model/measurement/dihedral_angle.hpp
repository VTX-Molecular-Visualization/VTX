#ifndef __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__
#define __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__

#include <app/component/chemistry/_fwd.hpp>
#include <app/component/generic/base_auto_delete.hpp>
#include <app/component/object3d/label.hpp>
#include <app/core/event/base_event_receiver_vtx.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/id.hpp>
#include <string>
#include <utility>

namespace VTX::Model::Measurement
{
	class DihedralAngle :
		public App::Component::Object3D::Label,
		public App::Core::Event::BaseEventReceiverVTX,
		public App::Component::Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView
			= App::Core::View::CallbackView<App::Component::Chemistry::Molecule, Model::Measurement::DihedralAngle>;

	  public:
		using AtomQuadruplet = std::tuple<const App::Component::Chemistry::Atom &,
										  const App::Component::Chemistry::Atom &,
										  const App::Component::Chemistry::Atom &,
										  const App::Component::Chemistry::Atom &>;

	  public:
		void setAtoms( const App::Component::Chemistry::Atom & p_firstAtom,
					   const App::Component::Chemistry::Atom & p_secondAtom,
					   const App::Component::Chemistry::Atom & p_thirdAtom,
					   const App::Component::Chemistry::Atom & p_fourthAtom );

		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		void _recomputeAABB( App::Component::Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const App::Component::Chemistry::Atom *> getAtoms() const { return _atoms; };
		const App::Component::Chemistry::Atom &					   getFirstAtom() const { return *_atoms[ 0 ]; }
		const App::Component::Chemistry::Atom &					   getSecondAtom() const { return *_atoms[ 1 ]; }
		const App::Component::Chemistry::Atom &					   getThirdAtom() const { return *_atoms[ 2 ]; }
		const App::Component::Chemistry::Atom &					   getFourthAtom() const { return *_atoms[ 3 ]; }

		float getAngle() const { return _dihedralAngle; }
		bool  isValid() const;

		void autoDelete() const override;

	  protected:
		DihedralAngle();
		DihedralAngle( const AtomQuadruplet & p_pair );

		~DihedralAngle();

		void _setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
								const App::Component::Chemistry::Atom & p_secondAtom,
								const App::Component::Chemistry::Atom & p_thirAtom,
								const App::Component::Chemistry::Atom & p_fourthAtom,
								const bool								p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const App::Component::Chemistry::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const App::Component::Chemistry::Molecule * const p_atom ) const;

		void _invalidate();

	  private:
		std::vector<const App::Component::Chemistry::Atom *> _atoms
			= std::vector<const App::Component::Chemistry::Atom *>();
		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();

		bool  _isAllAtomsOnSameMolecule = false;
		float _dihedralAngle			= 0.f;

		void _computeDihedralAngle( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::App::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule,
								const VTX::App::Core::Event::VTXEvent * const	  p_event );
	};
} // namespace VTX::Model::Measurement
#endif
