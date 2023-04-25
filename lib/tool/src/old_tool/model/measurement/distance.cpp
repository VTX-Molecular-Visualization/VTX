#include "tool/old_tool/model/measurement/distance.hpp"
#include "tool/old_tool/util/measurement.hpp"
#include <app/core/event/vtx_event.hpp>
#include <app/mvc.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/model/atom.hpp>
#include <app/model/molecule.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <util/math.hpp>

namespace VTX::Model::Measurement
{
	Distance::Distance() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE )
	{
		_atoms.resize( 2, nullptr );
		_moleculeViews.resize( 2, nullptr );

		_registerEvent( VTX::App::Event::Global::MOLECULE_REMOVED );
		_registerEvent( VTX::App::Event::Global::ATOM_REMOVED );
		_registerEvent( VTX::App::Event::Global::LABEL_REMOVED );

		setAutoNaming( true, false );
	}

	Distance::Distance( const AtomPair & p_pair ) : Distance()
	{
		_setAtomsInternal( p_pair.first, p_pair.second, false );
	}

	Distance::~Distance() {}

	void Distance::receiveEvent( const App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::ATOM_REMOVED )
		{
			const VTX::App::Core::Event::VTXEventArg<Model::Atom *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<Model::Atom *> &>( p_event );

			if ( _isLinkedToAtom( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
			}
		}
		else if ( p_event.name == VTX::App::Event::Global::MOLECULE_REMOVED )
		{
			const VTX::App::Core::Event::VTXEventArg<Model::Molecule *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<Model::Molecule *> &>( p_event );

			if ( _isLinkedToMolecule( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
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

	void Distance::setAtoms( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom )
	{
		_setAtomsInternal( p_firstAtom, p_secondAtom );
	}

	void Distance::_setAtomsInternal( const Model::Atom & p_firstAtom,
									  const Model::Atom & p_secondAtom,
									  const bool		  p_notify )
	{
		_cleanViews();

		_atoms[ 0 ] = &p_firstAtom;
		_atoms[ 1 ] = &p_secondAtom;

		_invalidateAABB();
		setPosition( ( _atoms[ 0 ]->getWorldPosition() + _atoms[ 1 ]->getWorldPosition() ) * 0.5f );

		_computeDistance( p_notify );

		_instantiateViewsOnMolecules();

		if ( hasAutoNaming() )
			_performAutoName( p_notify );
	}

	bool Distance::isValid() const
	{
		for ( const Model::Atom * const atomPtr : _atoms )
			if ( atomPtr == nullptr )
				return false;

		return true;
	}

	void Distance::_invalidate()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
			_atoms[ i ] = nullptr;
	}

	bool Distance::_isLinkedToAtom( const Model::Atom * const p_atom ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom == p_atom )
				return true;

		return false;
	}
	bool Distance::_isLinkedToMolecule( const Model::Molecule * const p_molecule ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom->getMoleculePtr() == p_molecule )
				return true;

		return false;
	}

	void Distance::_computeDistance( const bool p_notify )
	{
		const float newDistance
			= Util::Math::distance( _atoms[ 0 ]->getWorldPosition(), _atoms[ 1 ]->getWorldPosition() );

		if ( _distance != newDistance )
		{
			_distance = newDistance;

			if ( p_notify )
				_notifyDataChanged();
		}
	}

	void Distance::_recomputeAABB( Object3D::Helper::AABB & p_aabb )
	{
		p_aabb = Object3D::Helper::AABB();

		for ( const Model::Atom * const atom : _atoms )
		{
			if ( atom != nullptr )
				p_aabb.extend( atom->getWorldAABB() );
		}
	}

	void Distance::_instantiateViewsOnMolecules()
	{
		MoleculeView * const firstMoleculeView = VTX::MVC_MANAGER().instantiateView<MoleculeView>(
			_atoms[ 0 ]->getMoleculePtr(), getViewID( 0 ) );
		firstMoleculeView->setCallback( this, &Distance::_onMoleculeChange );
		_moleculeViews[ 0 ] = firstMoleculeView;

		if ( _atoms[ 0 ]->getMoleculePtr() != _atoms[ 1 ]->getMoleculePtr() )
		{
			MoleculeView * const secondMoleculeView = VTX::MVC_MANAGER().instantiateView<MoleculeView>(
				_atoms[ 1 ]->getMoleculePtr(), getViewID( 1 ) );
			secondMoleculeView->setCallback( this, &Distance::_onMoleculeChange );
			_moleculeViews[ 1 ] = firstMoleculeView;
		}
	}

	void Distance::_cleanViews()
	{
		for ( int i = 0; i < _moleculeViews.size(); i++ )
		{
			MoleculeView * const view = _moleculeViews[ i ];

			if ( view != nullptr )
			{
				VTX::MVC_MANAGER().deleteView( _atoms[ i ]->getMoleculePtr(), getViewID( i ) );
				_moleculeViews[ i ] = nullptr;
			}
		}
	}

	void Distance::_onMoleculeChange( const Model::Molecule * const					p_molecule,
									  const VTX::App::Core::Event::VTXEvent * const p_event )
	{
		bool recomputeDistance = false;
		if ( p_event->name == App::Event::Model::TRANSFORM_CHANGE )
		{
			// recompute only if the two atoms aren't in the same molecule
			recomputeDistance = _atoms[ 0 ]->getMoleculePtr() != _atoms[ 1 ]->getMoleculePtr();
			_invalidateAABB();
		}
		else if ( p_event->name == App::Event::Model::TRAJECTORY_FRAME_CHANGE )
		{
			recomputeDistance = true;
			_invalidateAABB();
		}

		if ( recomputeDistance )
		{
			_computeDistance( true );

			if ( hasAutoNaming() )
				_performAutoName();
		}
	}

	void Distance::_performAutoName( const bool p_notify )
	{
		std::string newName;

		if ( !isValid() )
		{
			newName = "...";
		}
		else
		{
			newName = _atoms[ 0 ]->getName() + "-" + _atoms[ 1 ]->getName() + " : "
					  + Util::Measurement::getDistanceString( *this );
		}

		if ( p_notify )
		{
			setName( newName );
		}
		else
		{
			std::string & nameRef = _getName();
			nameRef				  = newName;
		}
	}

	VTX::ID::VTX_ID Distance::getViewID( const int p_atomPos ) const
	{
		return VTX::MVC_MANAGER().generateViewID(
			VTX::ID::View::MEASUREMENT_ON_MOLECULE, std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
	}

	void Distance::autoDelete() const { VTX::MVC_MANAGER().deleteModel( this ); }

} // namespace VTX::Model::Measurement
