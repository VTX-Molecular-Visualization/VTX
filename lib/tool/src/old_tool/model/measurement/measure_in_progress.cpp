#include "tool/old_tool/model/measurement/measure_in_progress.hpp"
#include "tool/old_tool/util/measurement.hpp"
#include <algorithm>
#include <app/core/event/vtx_event.hpp>
#include <app/mvc.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/model/atom.hpp>
#include <app/model/molecule.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <util/math.hpp>
#include <variant>

namespace VTX::Model::Measurement
{
	void MeasureInProgress::PotentialTarget::setPosition( const QPoint & p_position )
	{
		_type			 = PotentialTargetType::POSITION;
		_target.position = p_position;
	}
	void MeasureInProgress::PotentialTarget::setAtom( const Model::Atom * const p_atom )
	{
		_type		 = PotentialTargetType::ATOM;
		_target.atom = p_atom;
	}
	void MeasureInProgress::PotentialTarget::setAtomPair( const Model::Atom * const p_firstAtom,
														  const Model::Atom * const p_secondAtom )
	{
		_type			 = PotentialTargetType::ATOMPAIR;
		_target.atomPair = { p_firstAtom, p_secondAtom };
	}

	MeasureInProgress::MeasureInProgress() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_ANGLE )
	{
		_atoms.reserve( 4 );
		_moleculeViews.reserve( 4 );

		_registerEvent( VTX::App::Event::Global::MOLECULE_REMOVED );
		_registerEvent( VTX::App::Event::Global::ATOM_REMOVED );
		_registerEvent( VTX::App::Event::Global::LABEL_REMOVED );

		setAutoNaming( false, false );
	}

	MeasureInProgress ::~MeasureInProgress() {}

	void MeasureInProgress::receiveEvent( const App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::ATOM_REMOVED )
		{
			const App::Core::Event::VTXEventArg<Model::Atom *> & castedEvent
				= dynamic_cast<const App::Core::Event::VTXEventArg<Model::Atom *> &>( p_event );

			if ( _isLinkedToAtom( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				clearPotentialTarget();
				clearAtoms();
			}
		}
		else if ( p_event.name == VTX::App::Event::Global::MOLECULE_REMOVED )
		{
			const App::Core::Event::VTXEventArg<Model::Molecule *> & castedEvent
				= dynamic_cast<const App::Core::Event::VTXEventArg<Model::Molecule *> &>( p_event );

			if ( _isLinkedToMolecule( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				clearPotentialTarget();
				clearAtoms();
			}
		}
		else if ( p_event.name == VTX::App::Event::Global::LABEL_REMOVED )
		{
			const App::Core::Event::VTXEventArg<Model::Label *> & castedEvent
				= dynamic_cast<const App::Core::Event::VTXEventArg<Model::Label *> &>( p_event );

			// TODO : Use a manager instead of managing scene from model
			if ( castedEvent.get() == this )
				_cleanViews();
		}
	}

	bool MeasureInProgress::applyPotentialTarget()
	{
		bool updated = false;
		switch ( _potentialNextTarget.getTargetType() )
		{
		case PotentialTargetType::NONE:
		case PotentialTargetType::POSITION: break;

		case PotentialTargetType::ATOM:
			addAtom( *_potentialNextTarget.getAtom() );
			updated = true;
			break;

		case PotentialTargetType::ATOMPAIR:
			const std::pair<const Model::Atom *, const Model::Atom *> & atomPair = _potentialNextTarget.getAtomPair();
			addAtom( *atomPair.first );
			addAtom( *atomPair.second );
			updated = true;
			break;
		}

		return updated;
	}

	void MeasureInProgress::addAtom( const Model::Atom & p_atom )
	{
		_atoms.emplace_back( &p_atom );

		bool  atomFromNewMolecule = true;
		Vec3f position			  = VEC3F_ZERO;

		for ( const Model::Atom * const atom : _atoms )
		{
			position += atom->getWorldPosition() / float( _atoms.size() );
			atomFromNewMolecule = atomFromNewMolecule && atom->getMoleculePtr() == p_atom.getMoleculePtr();
		}

		_invalidateAABB();
		setPosition( position );

		_isAllAtomsOnSameMolecule = _isAllAtomsOnSameMolecule && !atomFromNewMolecule;

		if ( atomFromNewMolecule )
		{
			MoleculeView * const moleculeView = VTX::MVC_MANAGER().instantiateView<MoleculeView>(
				p_atom.getMoleculePtr(), getViewID( int( _atoms.size() - 1 ) ) );

			moleculeView->setCallback( this, &MeasureInProgress::_onMoleculeChange );
			_moleculeViews.emplace_back( moleculeView );
		}
		else
		{
			_moleculeViews.emplace_back( nullptr );
		}

		_notifyDataChanged();
	}

	void MeasureInProgress::removeAtom( const Model::Atom & p_atom ) { _notifyDataChanged(); }

	void MeasureInProgress::clearAtoms()
	{
		_invalidateAABB();
		setPosition( VEC3F_ZERO );
		_cleanViews();
		_atoms.clear();
		_isAllAtomsOnSameMolecule = true;

		_notifyDataChanged();
	}

	const Model::Atom * const MeasureInProgress::getAtom( const int p_index ) const
	{
		if ( p_index < _atoms.size() )
			return _atoms[ p_index ];
		else
			return nullptr;
	}

	int MeasureInProgress::getAtomCount() const { return int( _atoms.size() ); }

	bool MeasureInProgress::contains( const Model::Atom & p_atom ) const
	{
		return std::find( _atoms.cbegin(), _atoms.cend(), &p_atom ) != _atoms.cend();
	}

	void MeasureInProgress::setPotentialNextTarget( const QPoint & p_position )
	{
		_potentialNextTarget.setPosition( p_position );
		_notifyDataChanged();
	}

	void MeasureInProgress::clearPotentialTarget()
	{
		_potentialNextTarget.clear();
		_notifyDataChanged();
	}

	void MeasureInProgress::setPotentialNextTarget( const Model::Atom & p_atom )
	{
		_potentialNextTarget.setAtom( &p_atom );
		_notifyDataChanged();
	}
	void MeasureInProgress::setPotentialNextTarget( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom )
	{
		_potentialNextTarget.setAtomPair( &p_firstAtom, &p_secondAtom );
		_notifyDataChanged();
	}

	bool MeasureInProgress::isValid() const
	{
		for ( const Model::Atom * const atomPtr : _atoms )
			if ( atomPtr == nullptr )
				return false;

		return true;
	}

	void MeasureInProgress::_invalidate()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
			_atoms[ i ] = nullptr;
	}

	bool MeasureInProgress::_isLinkedToAtom( const Model::Atom * const p_atom ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom == p_atom )
				return true;

		return false;
	}
	bool MeasureInProgress::_isLinkedToMolecule( const Model::Molecule * const p_molecule ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom->getMoleculePtr() == p_molecule )
				return true;

		return false;
	}

	void MeasureInProgress::_recomputeAABB( Object3D::Helper::AABB & p_aabb )
	{
		p_aabb = Object3D::Helper::AABB();

		for ( const Model::Atom * const atom : _atoms )
		{
			if ( atom != nullptr )
				p_aabb.extend( atom->getWorldAABB() );
		}
	}

	void MeasureInProgress::_cleanViews()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _moleculeViews[ i ] != nullptr )
			{
				VTX::MVC_MANAGER().deleteView( _atoms[ i ]->getMoleculePtr(), getViewID( i ) );
				_moleculeViews[ i ] = nullptr;
			}
		}
	}

	void MeasureInProgress::_onMoleculeChange( const Model::Molecule * const			p_molecule,
											   const App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == App::Event::Model::TRANSFORM_CHANGE )
		{
			_invalidateAABB();
		}
		else if ( p_event->name == App::Event::Model::TRAJECTORY_FRAME_CHANGE )
		{
			_invalidateAABB();
		}
	}

	VTX::ID::VTX_ID MeasureInProgress::getViewID( const int p_atomPos ) const
	{
		return VTX::MVC_MANAGER().generateViewID(
			VTX::ID::View::MEASUREMENT_ON_MOLECULE, std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
	}

	void MeasureInProgress::autoDelete() const { VTX::MVC_MANAGER().deleteModel( this ); }

} // namespace VTX::Model::Measurement
