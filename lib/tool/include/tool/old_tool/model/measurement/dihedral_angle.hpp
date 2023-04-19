#ifndef __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__
#define __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__

#include <app/event/base_event_receiver_vtx.hpp>
#include <app/event/vtx_event.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/model/label.hpp>
#include <app/old_app/view/callback_view.hpp>
#include <string>
#include <utility>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class DihedralAngle : public Model::Label, Event::BaseEventReceiverVTX, Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::DihedralAngle>;

	  public:
		using AtomQuadruplet
			= std::tuple<const Model::Atom &, const Model::Atom &, const Model::Atom &, const Model::Atom &>;

	  public:
		void setAtoms( const Model::Atom & p_firstAtom,
					   const Model::Atom & p_secondAtom,
					   const Model::Atom & p_thirdAtom,
					   const Model::Atom & p_fourthAtom );

		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void _recomputeAABB( Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const Model::Atom *> getAtoms() const { return _atoms; };
		const Model::Atom &					   getFirstAtom() const { return *_atoms[ 0 ]; }
		const Model::Atom &					   getSecondAtom() const { return *_atoms[ 1 ]; }
		const Model::Atom &					   getThirdAtom() const { return *_atoms[ 2 ]; }
		const Model::Atom &					   getFourthAtom() const { return *_atoms[ 3 ]; }

		float getAngle() const { return _dihedralAngle; }
		bool  isValid() const;

		void autoDelete() const override;

	  protected:
		DihedralAngle();
		DihedralAngle( const AtomQuadruplet & p_pair );

		~DihedralAngle();

		void _setAtomsInternal( const Model::Atom & p_firstAtom,
								const Model::Atom & p_secondAtom,
								const Model::Atom & p_thirAtom,
								const Model::Atom & p_fourthAtom,
								const bool			p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const Model::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const Model::Molecule * const p_atom ) const;

		void _invalidate();

	  private:
		std::vector<const Model::Atom *> _atoms			= std::vector<const Model::Atom *>();
		std::vector<MoleculeView *>		 _moleculeViews = std::vector<MoleculeView *>();

		bool  _isAllAtomsOnSameMolecule = false;
		float _dihedralAngle			= 0.f;

		void _computeDihedralAngle( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event );
	};
} // namespace VTX::Model::Measurement
#endif
