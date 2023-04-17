#include "app/old_app/model/representation/representation_library.hpp"
#include "app/old_app/action/action_manager.hpp"
#include "app/old_app/action/representation.hpp"
#include "app/old_app/event/event.hpp"
#include "app/old_app/event/event_manager.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include "app/old_app/setting.hpp"
#include "app/old_app/view/callback_view.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/old_app/worker/representation_loader.hpp"
#include "app/old_app/worker/worker_manager.hpp"
#include <string>

namespace VTX::Model::Representation
{
	RepresentationLibrary & RepresentationLibrary::get() { return VTXApp::get().getRepresentationLibrary(); };

	RepresentationLibrary::RepresentationLibrary() : BaseModel( VTX::ID::Model::MODEL_REPRESENTATION_LIBRARY )
	{
		Worker::RepresentationLibraryLoader * libraryLoader = new Worker::RepresentationLibraryLoader( *this );
		libraryLoader->activeNotify( false );
		libraryLoader->activeRepresentationRestoration( false );
		VTX_WORKER( libraryLoader );
	}

	RepresentationLibrary ::~RepresentationLibrary()
	{
		Action::Representation::SavePreset * const saveAction = new Action::Representation::SavePreset( *this );
		saveAction->setAsync( false );

		VTX_ACTION( saveAction );
		clear( false );
	}

	Representation * const RepresentationLibrary::getRepresentation( const int p_index )
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	const Representation * const RepresentationLibrary::getRepresentation( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	Representation * const RepresentationLibrary::getRepresentationByName( const std::string & p_name )
	{
		for ( Representation * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};
	const Representation * const RepresentationLibrary::getRepresentationByName( const std::string & p_name ) const
	{
		for ( const Representation * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};

	int RepresentationLibrary::getRepresentationIndex( const Representation * const p_representation ) const
	{
		for ( int i = 0; i < _representations.size(); i++ )
		{
			if ( _representations[ i ] == p_representation )
				return i;
		}

		return -1;
	}

	int RepresentationLibrary::getRepresentationCount() const { return int( _representations.size() ); };

	void RepresentationLibrary::addRepresentation( Representation * const p_representation, const bool p_notify )
	{
		_representations.emplace_back( p_representation );

		View::CallbackView<Representation, RepresentationLibrary> * const callbackView
			= MVC::MvcManager::get().instantiateView<View::CallbackView<Representation, RepresentationLibrary>>(
				p_representation, VTX::ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );

		callbackView->setCallback( this, &RepresentationLibrary::_onRepresentationChange );

		if ( p_notify )
		{
			_notifyDataChanged();

			const int newRepresentationIndex = int( _representations.size() ) - 1;
			VTX_EVENT( new Event::VTXEventValue<int>( Event::Global::REPRESENTATION_ADDED, newRepresentationIndex ) );
		}
	};
	void RepresentationLibrary::copyRepresentation( const int p_index, const bool p_notify )
	{
		Representation * const sourceRepresentation = _representations[ p_index ];
		Representation * const copiedRepresentation
			= MVC::MvcManager::get().instantiateModel<Representation>( sourceRepresentation->getRepresentationType() );

		copiedRepresentation->copyDataFrom( *sourceRepresentation );

		copiedRepresentation->setName( getValidName( sourceRepresentation->getName() ) );

		addRepresentation( copiedRepresentation, p_notify );
	}
	Representation * RepresentationLibrary::removeRepresentation( const int p_index, const bool p_notify )
	{
		Representation * removedRepresentation;

		if ( 0 <= p_index && p_index < _representations.size() )
		{
			removedRepresentation = _representations[ p_index ];

			MVC::MvcManager::get().deleteView( _representations[ p_index ],
											   VTX::ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );
			_representations.erase( _representations.begin() + p_index );

			if ( p_notify )
				_notifyDataChanged();

			VTX_EVENT( new Event::VTXEventValue<int>( Event::Global::REPRESENTATION_REMOVED, p_index ) );
		}
		else
		{
			removedRepresentation = nullptr;
		}

		return removedRepresentation;
	};

	bool RepresentationLibrary::canDeleteRepresentation( Representation * const p_representation ) const
	{
		return p_representation != _defaultRepresentation;
	}

	void RepresentationLibrary::deleteRepresentation( const Representation * const p_representation,
													  const bool				   p_notify )
	{
		const int representationIndex = getRepresentationIndex( p_representation );
		deleteRepresentation( representationIndex, p_notify );
	}

	void RepresentationLibrary::deleteRepresentation( const int p_index, const bool p_notify )
	{
		const Representation * const representationToDelete = removeRepresentation( p_index, p_notify );

		if ( representationToDelete != nullptr )
			MVC::MvcManager::get().deleteModel( representationToDelete );
	}

	void RepresentationLibrary::setDefaultRepresentation( const int p_representationIndex, const bool p_notify )
	{
		if ( 0 <= p_representationIndex && p_representationIndex < _representations.size() )
		{
			_defaultRepresentation = _representations[ p_representationIndex ];
		}
		else if ( _representations.size() > 0 )
		{
			_defaultRepresentation = getRepresentationByName( VTX_SETTING().REPRESENTATION_DEFAULT_NAME );

			if ( _defaultRepresentation == nullptr )
			{
				_defaultRepresentation = _representations[ 0 ];
			}
		}
		else
		{
			return;
		}

		VTX_SETTING().setDefaultRepresentationIndex( p_representationIndex );

		if ( p_notify )
			_notifyDataChanged();
	}
	int RepresentationLibrary::getDefaultRepresentationIndex() const
	{
		return getRepresentationIndex( _defaultRepresentation );
	}
	Representation * RepresentationLibrary::getDefaultRepresentation() { return _defaultRepresentation; }

	Representation * RepresentationLibrary::getDefaultRepresentation( const CATEGORY_ENUM & p_categoryEnum )
	{
		const int representationIndex = VTX_SETTING().getDefaultRepresentationIndexPerCategory( p_categoryEnum );

		Representation * res = getRepresentation( representationIndex );

		if ( res == nullptr )
			res = getDefaultRepresentation();

		return res;
	}

	void RepresentationLibrary::clear( const bool p_notify )
	{
		while ( _representations.size() > 0 )
		{
			deleteRepresentation( int( _representations.size() - 1 ), p_notify );
		}
	}

	void RepresentationLibrary::resetToDefault( const bool p_notify )
	{
		clear( false );
		applyDefault( false );

		const std::string & defaultRepresentationName = VTX_SETTING().getTmpRepresentationDefaultName();
		Representation *	defaultRepresentation	  = getRepresentationByName( defaultRepresentationName );

		if ( defaultRepresentation == nullptr && _representations.size() > 0 )
			defaultRepresentation = _representations[ 0 ];

		setDefaultRepresentation( getRepresentationIndex( defaultRepresentation ), false );

		if ( p_notify )
			_notifyDataChanged();
	}

	std::string RepresentationLibrary::getValidName( const std::string & p_name ) const
	{
		const std::string & defaultStr = p_name;
		std::string			validName  = defaultStr;

		int counter = 2;
		while ( !isValidName( validName ) )
		{
			validName = defaultStr + " (" + std::to_string( counter ) + ")";
			counter++;
		}

		return validName;
	}
	bool RepresentationLibrary::isValidName( const std::string & p_name ) const
	{
		for ( const Representation * const representation : _representations )
			if ( representation->getName() == p_name )
				return false;

		return true;
	}

	bool RepresentationLibrary::isExistingName( const std::string & p_name ) const
	{
		return getRepresentationByName( p_name ) != nullptr;
	}

	void RepresentationLibrary::applyDefault( const bool p_notify )
	{
		Generic::REPRESENTATION representationType;
		std::string				name;

		representationType = Generic::REPRESENTATION::BALL_AND_STICK;
		name			   = "Ball And Stick";
		if ( isExistingName( name ) == false )
		{
			Representation * const ballsAndsticks
				= MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			ballsAndsticks->setName( name );
			ballsAndsticks->setQuickAccess( true );
			ballsAndsticks->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( ballsAndsticks, p_notify );
		}

		representationType = Generic::REPRESENTATION::CARTOON;
		name			   = "Cartoon";
		if ( isExistingName( name ) == false )
		{
			Representation * const cartoon
				= MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			cartoon->setName( name );
			cartoon->setQuickAccess( true );
			cartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( cartoon, p_notify );
		}

		representationType = Generic::REPRESENTATION::STICK_AND_CARTOON;
		name			   = "Cartoon + Stick";
		if ( isExistingName( name ) == false )
		{
			Representation * const stickAndCartoon
				= MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			stickAndCartoon->setName( name );
			stickAndCartoon->setQuickAccess( true );
			stickAndCartoon->getData().setCylinderRadius( 0.15f );
			stickAndCartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( stickAndCartoon, p_notify );
		}

		representationType = Generic::REPRESENTATION::SAS;
		name			   = "SAS";
		if ( isExistingName( name ) == false )
		{
			Representation * const sas = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			sas->setName( name );
			sas->setQuickAccess( true );
			sas->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( sas, p_notify );
		}

		representationType = Generic::REPRESENTATION::SES;
		name			   = "SES";
		if ( isExistingName( name ) == false )
		{
			Representation * const ses = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			ses->setName( name );
			ses->setQuickAccess( true );
			ses->getData().setColorMode( Generic::COLOR_MODE::PROTEIN );
			addRepresentation( ses, p_notify );
		}

		representationType = Generic::REPRESENTATION::STICK;
		name			   = "Stick";
		if ( isExistingName( name ) == false )
		{
			Representation * const stick
				= MVC::MvcManager::get().instantiateModel<Representation, Generic::REPRESENTATION>(
					representationType );
			stick->setName( name );
			stick->setQuickAccess( true );
			stick->getData().setCylinderRadius( 0.15f );
			stick->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
			addRepresentation( stick, p_notify );
		}

		representationType = Generic::REPRESENTATION::VAN_DER_WAALS;
		name			   = "VdW";
		if ( isExistingName( name ) == false )
		{
			Representation * const vdw = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
			vdw->setName( name );
			vdw->setQuickAccess( true );
			vdw->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
			vdw->getData().setSphereRadius( 0 );
			addRepresentation( vdw, p_notify );
		}
	}

	void RepresentationLibrary::_onRepresentationChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
			_notifyViews( new Event::VTXEvent( Event::Model::SUBITEM_DATA_CHANGE ) );
		else
			_notifyViews( new Event::VTXEvent( *p_event ) );
	}

} // namespace VTX::Model::Representation
