#include "distance.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "util/math.hpp"
#include "util/measurement.hpp"
#include <string>

namespace VTX::Model::Measurement
{
	Distance::Distance() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE )
	{
		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::ATOM_REMOVED );
		_registerEvent( Event::Global::LABEL_REMOVED );

		setAutoNaming( true, false );
		_moleculeViews.reserve( 2 );
	}

	Distance::Distance( const AtomPair & p_pair ) : Distance()
	{
		_setAtomsInternal( p_pair.first, p_pair.second, false );
	}

	Distance ::~Distance() {}

	VTX::ID::VTX_ID Distance::getViewID( const int p_atomPos ) const
	{
		return VTX::ID::View::MEASUREMENT_DISTANCE_ON_MOLECULE + std::to_string( getId() ) + '_'
			   + std::to_string( p_atomPos );
	}

	void Distance::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::ATOM_REMOVED )
		{
			const Event::VTXEventPtr<Model::Atom> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Atom> &>( p_event );

			if ( castedEvent.ptr == _firstAtom || castedEvent.ptr == _secondAtom )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				MVC::MvcManager::get().deleteModel( this );
			}
		}
		else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

			if ( castedEvent.ptr == _firstAtom->getMoleculePtr() || castedEvent.ptr == _secondAtom->getMoleculePtr() )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				MVC::MvcManager::get().deleteModel( this );
			}
		}
		else if ( p_event.name == Event::Global::LABEL_REMOVED )
		{
			const Event::VTXEventPtr<Model::Label> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Label> &>( p_event );

			// TODO : Use a manager instead of managing scene from model
			if ( castedEvent.ptr == this )
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

		_firstAtom	= &p_firstAtom;
		_secondAtom = &p_secondAtom;

		_invalidateAABB();
		setPosition( ( _firstAtom->getWorldPosition() + _secondAtom->getWorldPosition() ) * 0.5f );

		_computeDistance( p_notify );

		_instantiateViewsOnMolecules();

		if ( hasAutoNaming() )
			_performAutoName( p_notify );
	}

	void Distance::_computeDistance( const bool p_notify )
	{
		const float newDistance
			= Util::Math::distance( _firstAtom->getWorldPosition(), _secondAtom->getWorldPosition() );

		if ( _distance != newDistance )
		{
			_distance = newDistance;

			if ( p_notify )
				_notifyDataChanged();
		}
	}

	void Distance::_recomputeAABB( Math::AABB & p_aabb )
	{
		p_aabb = Math::AABB();
		p_aabb.extend( _firstAtom->getWorldAABB() );
		p_aabb.extend( _secondAtom->getWorldAABB() );
	}

	void Distance::_instantiateViewsOnMolecules()
	{
		MoleculeView * const firstMoleculeView
			= MVC::MvcManager::get().instantiateView<MoleculeView>( _firstAtom->getMoleculePtr(), getViewID( 0 ) );
		firstMoleculeView->setCallback( this, &Distance::_onMoleculeChange );
		_moleculeViews.emplace_back( firstMoleculeView );

		if ( _firstAtom->getMoleculePtr() != _secondAtom->getMoleculePtr() )
		{
			MoleculeView * const secondMoleculeView
				= MVC::MvcManager::get().instantiateView<MoleculeView>( _secondAtom->getMoleculePtr(), getViewID( 1 ) );
			secondMoleculeView->setCallback( this, &Distance::_onMoleculeChange );
			_moleculeViews.emplace_back( secondMoleculeView );
		}
	}

	void Distance::_cleanViews()
	{
		if ( _moleculeViews.size() > 0 )
		{
			MVC::MvcManager::get().deleteView( _firstAtom->getMoleculePtr(), getViewID( 0 ) );
		}

		if ( _moleculeViews.size() > 1 )
		{
			MVC::MvcManager::get().deleteView( _secondAtom->getMoleculePtr(), getViewID( 1 ) );
		}

		_moleculeViews.clear();
	}

	void Distance::_onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event )
	{
		bool recomputeDistance = false;
		if ( p_event->name == Event::Model::TRANSFORM_CHANGE )
		{
			// recompute only if the two atoms aren't in the same molecule
			recomputeDistance = _firstAtom->getMoleculePtr() != _secondAtom->getMoleculePtr();
			_invalidateAABB();
		}
		else if ( p_event->name == Event::Model::TRAJECTORY_FRAME_CHANGE )
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

		if ( _firstAtom == nullptr || _secondAtom == nullptr )
		{
			newName = "...";
		}
		else
		{
			newName = _firstAtom->getName() + "-" + _secondAtom->getName() + " : "
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

} // namespace VTX::Model::Measurement
