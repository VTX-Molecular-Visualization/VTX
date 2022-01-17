#include "distance.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "mvc/mvc_manager.hpp"
#include "util/math.hpp"
#include "util/measurement.hpp"

namespace VTX::Model::Measurement
{
	Distance::Distance() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE )
	{
		_registerEvent( Event::Global::ATOM_REMOVED );
		_registerEvent( Event::Global::MOLECULE_TRAJECTORY_FRAME_CHANGE );

		setAutoNaming( true, false );
	}

	Distance::Distance( const AtomPair & p_pair ) : Distance()
	{
		_setAtomsInternal( p_pair.first, p_pair.second, false );
	}

	void Distance::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::ATOM_REMOVED )
		{
			const Event::VTXEventPtr<Model::Atom> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Atom> &>( p_event );

			if ( castedEvent.ptr == _firstAtom || castedEvent.ptr == _secondAtom )
			{
				VTX_EVENT( new Event::VTXEventPtr<Model::Label>( Event::Global::LABEL_REMOVED, this ) );
				MVC::MvcManager::get().deleteModel( this );
			}
		}
		else if ( p_event.name == Event::Global::MOLECULE_TRAJECTORY_FRAME_CHANGE )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

			if ( castedEvent.ptr == _firstAtom->getMoleculePtr() || castedEvent.ptr == _secondAtom->getMoleculePtr() )
			{
				_computeDistance( true );

				if ( hasAutoNaming() )
					_performAutoName();
			}
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
		_firstAtom	= &p_firstAtom;
		_secondAtom = &p_secondAtom;

		_invalidateAABB();

		setPosition( ( _firstAtom->getWorldPosition() + _secondAtom->getWorldPosition() ) * 0.5f );

		_computeDistance( p_notify );

		if ( hasAutoNaming() )
			_performAutoName( p_notify );
	}

	void Distance::_computeDistance( const bool p_notify )
	{
		_distance = Util::Math::distance( _firstAtom->getWorldPosition(), _secondAtom->getWorldPosition() );
		if ( p_notify )
			_notifyDataChanged();
	}

	void Distance::_recomputeAABB( Math::AABB & p_aabb )
	{
		p_aabb = Math::AABB();
		p_aabb.extend( _firstAtom->getWorldAABB() );
		p_aabb.extend( _secondAtom->getWorldAABB() );
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
