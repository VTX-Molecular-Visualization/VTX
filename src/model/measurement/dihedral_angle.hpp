#ifndef __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__
#define __VTX_MODEL_MEASUREMENT_DIHEDRAL_ANGLE__

#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "id.hpp"
#include "model/label.hpp"
#include "view/callback_view.hpp"
#include <string>
#include <utility>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class DihedralAngle : public Model::Label, Event::BaseEventReceiverVTX
	{
		VTX_MODEL
		//
		//	  private:
		//		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::DihedralAngle>;
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
		//	  protected:
		//		DihedralAngle();
		//		DihedralAngle( const AtomPair & p_pair );
		//
		//		~DihedralAngle();
		//
		//		void _setAtomsInternal( const Model::Atom & p_firstAtom,
		//								const Model::Atom & p_secondAtom,
		//								const bool			p_notify = true );
		//
		//		void _performAutoName( const bool p_notify = true ) override;
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
