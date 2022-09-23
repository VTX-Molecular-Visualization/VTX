#ifndef __VTX_MODEL_MEASUREMENT_ANGLE__
#define __VTX_MODEL_MEASUREMENT_ANGLE__

#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "generic/base_auto_delete.hpp"
#include "id.hpp"
#include "model/label.hpp"
#include "view/callback_view.hpp"
#include <string>
#include <tuple>
#include <vector>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class Angle : public Model::Label, Event::BaseEventReceiverVTX, Generic::BaseAutoDelete
	{
		VTX_MODEL

	  private:
		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::Angle>;

	  public:
		using AtomTriplet = std::tuple<const Model::Atom &, const Model::Atom &, const Model::Atom &>;

	  public:
		void setAtoms( const Model::Atom & p_firstAtom,
					   const Model::Atom & p_secondAtom,
					   const Model::Atom & p_thirdAtom );

		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void _recomputeAABB( Math::AABB & p_aabb ) override;

		const std::vector<const Model::Atom *> getAtoms() const { return _atoms; };
		const Model::Atom &					   getFirstAtom() const { return *_atoms[ 0 ]; }
		const Model::Atom &					   getSecondAtom() const { return *_atoms[ 1 ]; }
		const Model::Atom &					   getThirdAtom() const { return *_atoms[ 2 ]; }

		float getAngle() const { return _angle; }
		bool  isValid() const;

		void autoDelete() const override;

		void _invalidate();

	  protected:
		Angle();
		Angle( const AtomTriplet & p_triplet );

		~Angle();

		void _setAtomsInternal( const Model::Atom & p_firstAtom,
								const Model::Atom & p_secondAtom,
								const Model::Atom & p_thirdAtom,
								const bool			p_notify = true );

		void _performAutoName( const bool p_notify = true ) override;

		bool _isLinkedToAtom( const Model::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const Model::Molecule * const p_atom ) const;

	  private:
		std::vector<const Model::Atom *> _atoms			= std::vector<const Model::Atom *>();
		std::vector<MoleculeView *>		 _moleculeViews = std::vector<MoleculeView *>();

		float _angle					= 0.f;
		bool  _isAllAtomsOnSameMolecule = false;

		void _computeAngle( const bool p_notify = true );
		void _instantiateViewsOnMolecules();
		void _cleanViews();

		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event );
	};

} // namespace VTX::Model::Measurement
#endif
