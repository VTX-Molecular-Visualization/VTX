#ifndef __VTX_MODEL_MEASUREMENT_MEASURE_IN_PROGRESS__
#define __VTX_MODEL_MEASUREMENT_MEASURE_IN_PROGRESS__

#include <QPoint>
#include <app/old/component/chemistry/_fwd.hpp>
#include <app/old/component/generic/base_auto_delete.hpp>
#include <app/old/component/object3d/label.hpp>
#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/core/view/callback_view.hpp>
#include <app/old/id.hpp>
#include <string>
#include <vector>

namespace VTX::Model::Measurement
{
	class MeasureInProgress :
		public App::Old::Component::Object3D::Label,
		public App::Old::Core::Event::BaseEventReceiverVTX,
		public App::Old::Component::Generic::BaseAutoDelete
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
		using MoleculeView
			= App::Old::Core::View::CallbackView<App::Old::Component::Chemistry::Molecule, Model::Measurement::MeasureInProgress>;

		class PotentialTarget
		{
		  private:
			union TargetItem
			{
				TargetItem() { position = QPoint( 0, 0 ); }
				TargetItem( const App::Old::Component::Chemistry::Atom * const p_atom ) : atom( p_atom ) {};
				TargetItem( const App::Old::Component::Chemistry::Atom * const p_firstAtom,
							const App::Old::Component::Chemistry::Atom * const p_secondAtom ) :
					atomPair( { p_firstAtom, p_secondAtom } ) {};
				TargetItem( const QPoint & p_position ) : position( p_position ) {};

				const App::Old::Component::Chemistry::Atom *														atom;
				std::pair<const App::Old::Component::Chemistry::Atom *, const App::Old::Component::Chemistry::Atom *> atomPair;
				QPoint																						position;
			};

		  public:
			PotentialTarget() {}

			PotentialTargetType							  getTargetType() const { return _type; }
			const App::Old::Component::Chemistry::Atom * const getAtom() const { return _target.atom; }
			const std::pair<const App::Old::Component::Chemistry::Atom *, const App::Old::Component::Chemistry::Atom *> &
			getAtomPair() const
			{
				return _target.atomPair;
			}
			const QPoint & getPosition() const { return _target.position; }

			void setAtom( const App::Old::Component::Chemistry::Atom * const p_atom );
			void setAtomPair( const App::Old::Component::Chemistry::Atom * const p_firstAtom,
							  const App::Old::Component::Chemistry::Atom * const p_secondAtom );
			void setPosition( const QPoint & p_position );
			void clear() { _type = PotentialTargetType::NONE; }

		  private:
			PotentialTargetType _type = PotentialTargetType::NONE;
			union TargetItem	_target;
		};

	  public:
		void addAtom( const App::Old::Component::Chemistry::Atom & p_atom );
		void removeAtom( const App::Old::Component::Chemistry::Atom & p_atom );
		void clearAtoms();

		const App::Old::Component::Chemistry::Atom * const getAtom( const int p_index ) const;
		int											  getAtomCount() const;
		bool										  contains( const App::Old::Component::Chemistry::Atom & p_atom ) const;

		const PotentialTargetType getPotentialNextTargetType() const { return _potentialNextTarget.getTargetType(); }
		const App::Old::Component::Chemistry::Atom * const getPotentialNextAtom() const
		{
			return _potentialNextTarget.getAtom();
		}
		const std::pair<const App::Old::Component::Chemistry::Atom *, const App::Old::Component::Chemistry::Atom *> &
		getPotentialNextAtomPair() const
		{
			return _potentialNextTarget.getAtomPair();
		}
		const QPoint & getPotentialNextPosition() const { return _potentialNextTarget.getPosition(); }

		void setPotentialNextTarget( const App::Old::Component::Chemistry::Atom & p_atom );
		void setPotentialNextTarget( const App::Old::Component::Chemistry::Atom & p_firstAtom,
									 const App::Old::Component::Chemistry::Atom & p_secondAtom );
		void setPotentialNextTarget( const QPoint & p_position );

		bool applyPotentialTarget();
		void clearPotentialTarget();

		void receiveEvent( const App::Old::Core::Event::VTXEvent & p_event ) override;

		void _recomputeAABB( App::Old::Component::Object3D::Helper::AABB & p_aabb ) override;

		const std::vector<const App::Old::Component::Chemistry::Atom *> getAtoms() const { return _atoms; };
		bool													   isValid() const;

		void autoDelete() const override;

		void _invalidate();

	  protected:
		MeasureInProgress();
		~MeasureInProgress();

		bool _isLinkedToAtom( const App::Old::Component::Chemistry::Atom * const p_atom ) const;
		bool _isLinkedToMolecule( const App::Old::Component::Chemistry::Molecule * const p_atom ) const;

	  private:
		std::vector<const App::Old::Component::Chemistry::Atom *> _atoms
			= std::vector<const App::Old::Component::Chemistry::Atom *>();
		std::vector<MoleculeView *> _moleculeViews = std::vector<MoleculeView *>();

		bool			_isAllAtomsOnSameMolecule = false;
		QPoint			_cursorPos;
		PotentialTarget _potentialNextTarget = PotentialTarget();

		void _cleanViews();

		VTX::App::Old::VTX_ID getViewID( const int p_atomPos ) const;

		void _onMoleculeChange( const App::Old::Component::Chemistry::Molecule * const p_molecule,
								const App::Old::Core::Event::VTXEvent * const		  p_event );
	};

} // namespace VTX::Model::Measurement
#endif
