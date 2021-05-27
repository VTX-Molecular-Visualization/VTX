#include "representation_library.hpp"
#include "action/action_manager.hpp"
#include "action/representation.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"
#include "view/callback_view.hpp"
#include "vtx_app.hpp"
#include "worker/representation_loader.hpp"
#include "worker/worker_manager.hpp"
#include <string>

namespace VTX::Model::Representation
{
	RepresentationLibrary & RepresentationLibrary::get() { return VTXApp::get().getRepresentationLibrary(); };

	RepresentationLibrary::RepresentationLibrary() : BaseModel( ID::Model::MODEL_REPRESENTATION_LIBRARY )
	{
		Worker::RepresentationLibraryLoader * libraryLoader = new Worker::RepresentationLibraryLoader( *this );
		libraryLoader->activeNotify( false );
		VTX_WORKER( libraryLoader );

		if ( _representations.size() <= 0 )
		{
			_init();
		}

		_defaultRepresentation = getRepresentationByName( VTX_SETTING().getTmpRepresentationDefaultName() );

		if ( _defaultRepresentation == nullptr )
		{
			_defaultRepresentation = _representations[ Setting::REPRESENTATION_DEFAULT_INDEX ];
		}

		VTX_SETTING().setDefaultRepresentationIndex( getRepresentationIndex( _defaultRepresentation ) );
		VTX_SETTING().setTmpRepresentationDefaultName( "" );
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
				p_representation, ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );

		callbackView->setCallback( this, &RepresentationLibrary::_onRepresentationChange );

		if ( p_notify )
			_notifyDataChanged();

		const int newRepresentationIndex = int( _representations.size() ) - 1;
		VTX_EVENT( new Event::VTXEventValue<int>( Event::Global::REPRESENTATION_ADDED, newRepresentationIndex ) );
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

			MVC::MvcManager::get().deleteView( _representations[ p_index ], ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );
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

	void RepresentationLibrary::setDefaultRepresentation( const int p_representationIndex )
	{
		_defaultRepresentation = _representations[ p_representationIndex ];
		VTX_SETTING().setDefaultRepresentationIndex( p_representationIndex );
		_notifyDataChanged();
	}
	int RepresentationLibrary::getDefaultRepresentationIndex() const
	{
		return getRepresentationIndex( _defaultRepresentation );
	}
	Representation * RepresentationLibrary::getDefaultRepresentation() { return _defaultRepresentation; }

	void RepresentationLibrary::clear( const bool p_notify )
	{
		while ( _representations.size() > 0 )
		{
			deleteRepresentation( int( _representations.size() - 1 ), p_notify );
		}
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

	void RepresentationLibrary::_init()
	{
		Generic::REPRESENTATION representationType = Generic::REPRESENTATION::STICK;

		Representation * const stick
			= MVC::MvcManager::get().instantiateModel<Representation, Generic::REPRESENTATION>( representationType );
		stick->setName( "Stick" );
		stick->getData().setCylinderRadius( 0.15f );
		stick->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
		stick->setQuickAccess( true );
		addRepresentation( stick, false );

		representationType		   = Generic::REPRESENTATION::VAN_DER_WAALS;
		Representation * const vdw = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
		vdw->setName( "VdW" );
		vdw->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
		vdw->getData().setSphereRadius( 0 );
		addRepresentation( vdw, false );

		representationType		   = Generic::REPRESENTATION::SAS;
		Representation * const sas = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
		sas->setName( "SAS" );
		sas->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
		sas->setQuickAccess( true );
		addRepresentation( sas, false );

		representationType = Generic::REPRESENTATION::BALL_AND_STICK;
		Representation * const ballsAndsticks
			= MVC::MvcManager::get().instantiateModel<Representation>( representationType );
		ballsAndsticks->setName( "Ball And Stick" );
		ballsAndsticks->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
		addRepresentation( ballsAndsticks, false );

		representationType			   = Generic::REPRESENTATION::CARTOON;
		Representation * const cartoon = MVC::MvcManager::get().instantiateModel<Representation>( representationType );
		cartoon->setName( "Cartoon" );
		cartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
		cartoon->setQuickAccess( true );
		addRepresentation( cartoon, false );

		representationType = Generic::REPRESENTATION::STICK_AND_CARTOON;
		Representation * const stickAndCartoon
			= MVC::MvcManager::get().instantiateModel<Representation>( representationType );
		stickAndCartoon->setName( "Stick + Cartoon" );
		stickAndCartoon->getData().setCylinderRadius( 0.15f );
		stickAndCartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
		addRepresentation( stickAndCartoon, false );
	}

	void RepresentationLibrary::_onRepresentationChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
			_notifyViews( new Event::VTXEvent( Event::Model::SUBITEM_DATA_CHANGE ) );
		else
			_notifyViews( new Event::VTXEvent( *p_event ) );
	}

} // namespace VTX::Model::Representation
