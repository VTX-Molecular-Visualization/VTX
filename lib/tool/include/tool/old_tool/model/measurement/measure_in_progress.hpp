#ifndef __VTX_MODEL_MEASUREMENT_MEASURE_IN_PROGRESS__
#define __VTX_MODEL_MEASUREMENT_MEASURE_IN_PROGRESS__

#include <QPoint>
#include <app/event/base_event_receiver_vtx.hpp>
#include <app/event/vtx_event.hpp>
#include <app/old_app/generic/base_auto_delete.hpp>
#include <app/old_app/id.hpp>
#include <app/model/label.hpp>
#include <app/view/callback_view.hpp>
#include <string>
#include <vector>

namespace VTX::Model
{
	class Atom;
	class Molecule;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class MeasureInProgress : public Model::Label, Event::BaseEventReceiverVTX, Generic::BaseAutoDelete
	{
		VTX_MODEL

	  public:
		enum class PotentialTargetType : int
		{
			NONE,
			POSITION,
			ATOM,
			ATOMPAIR
		};

	  private:
		using MoleculeView = View::CallbackView<Model::Molecule, Model::Measurement::MeasureInProgress>;

		class PotentialTarget
		{
		  private:
			union TargetItem
			{
				TargetItem() { position = QPoint( 0, 0 ); }
				TargetItem( const Model::Atom * const p_atom ) : atom( p_atom ) {};
				TargetItem( const Model::Atom * const p_firstAtom, const Model::Atom * const p_secondAtom ) :
					atomPair( { p_firstAtom, p_secondAtom } ) {};
				TargetItem( const QPoint & p_position ) : position( p_position ) {};

				const Model::Atom *									atom;
				std::pair<const Model::Atom *, const Model::Atom *> atomPair;
				QPoint												position;
			};

		  public:
			PotentialTarget() {}

			PotentialTargetType											getTargetType() const { return _type; }
			const Model::Atom * const									getAtom() const { return _target.atom; }
			const std::pair<const Model::Atom *, const Model::Atom *> & getAtomPair() const { return _target.atomPair; }
			const QPoint &												getPosition() const { return _target.position; }

			void setAtom( const Model::Atom * const p_atom );
			void setAtomPair( const Model::Atom * const p_firstAtom, const Model::Atom * const p_secondAtom );
			void setPosition( const QPoint & p_position );
			void clear() { _type = PotentialTargetType::NONE; }

		  private:
			PotentialTargetType _type = PotentialTargetType::NONE;
			union TargetItem	_target;
		};

	  public:
		void addAtom( const Model::Atom & p_atom );
		void removeAtom( const Model::Atom & p_atom );
		void clearAtoms();

		const Model::Atom * const getAtom( const int p_index ) const;
		int						  getAtomCount() const;
		bool					  contains( const Model::Atom & p_atom ) const;

		const PotentialTargetType getPotentialNextTargetType() const { return _potentialNextTarget.getTargetType(); }
		const Model::Atom * const getPotentialNextAtom() const { return _potentialNextTarget.getAtom(); }
		const std::pair<const Model::Atom *, const Model::Atom *> & getPotentialNextAtomPair() const
		{
			return _potentialNextTarget.getAtomPair();
		}
		const QPoint & getPotentialNextPosition() const { return _potentialNextTarget.getPosition(); }

		void setPotentialNextTarget( const Model::Atom & p_atom );
		void setPotentialNextTarget( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom );
		void setPotentialNextTarget( const QPoint & p_position );

		bool applyPotentialTarget();
		void clearPotentialTarget();

		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void _recomputeAABB( Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const Model::Atom *> getAtoms() const { return _atoms; };
		bool								   isValid() const;

		void autoDelete() const override;

		void _invalidate();

	  protected:
		MeasureInProgress();
		~MeasureInProgress();

		bool _isLinkedToAtom( const Model::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const Model::Molecule * const p_atom ) const;

	  private:
		std::vector<const Model::Atom *> _atoms			= std::vector<const Model::Atom *>();
		std::vector<MoleculeView *>		 _moleculeViews = std::vector<MoleculeView *>();

		bool			_isAllAtomsOnSameMolecule = false;
		QPoint			_cursorPos;
		PotentialTarget _potentialNextTarget = PotentialTarget();

		void _cleanViews();

		VTX::ID::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event );
	};

} // namespace VTX::Model::Measurement
#endif
