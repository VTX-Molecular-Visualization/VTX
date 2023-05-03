#include "app/application/representation/representation_library.hpp"
#include "app/action/representation.hpp"
#include "app/application/setting.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/core/view/callback_view.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/id.hpp"
#include "app/internal/worker/representation_loader.hpp"
#include "app/manager/action_manager.hpp"
#include "app/mvc.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker.hpp"
#include <string>

namespace VTX::App::Application::Representation
{
	RepresentationLibrary & RepresentationLibrary::get() { return VTXApp::get().getRepresentationLibrary(); };

	RepresentationLibrary::RepresentationLibrary() : BaseModel( App::ID::Model::MODEL_REPRESENTATION_LIBRARY )
	{
		Internal::Worker::RepresentationLibraryLoader * libraryLoader
			= new Internal::Worker::RepresentationLibraryLoader( *this );
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

	RepresentationPreset * const RepresentationLibrary::getRepresentation( const int p_index )
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	const RepresentationPreset * const RepresentationLibrary::getRepresentation( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	RepresentationPreset * const RepresentationLibrary::getRepresentationByName( const std::string & p_name )
	{
		for ( RepresentationPreset * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};
	const RepresentationPreset * const RepresentationLibrary::getRepresentationByName(
		const std::string & p_name ) const
	{
		for ( const RepresentationPreset * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};

	int RepresentationLibrary::getRepresentationIndex( const RepresentationPreset * const p_representation ) const
	{
		for ( int i = 0; i < _representations.size(); i++ )
		{
			if ( _representations[ i ] == p_representation )
				return i;
		}

		return -1;
	}

	int RepresentationLibrary::getRepresentationCount() const { return int( _representations.size() ); };

	void RepresentationLibrary::addRepresentation( RepresentationPreset * const p_representation, const bool p_notify )
	{
		_representations.emplace_back( p_representation );

		Core::View::CallbackView<RepresentationPreset, RepresentationLibrary> * const callbackView
			= VTX::MVC_MANAGER().instantiateView<Core::View::CallbackView<RepresentationPreset, RepresentationLibrary>>(
				p_representation, VTX::App::ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );

		callbackView->setCallback( this, &RepresentationLibrary::_onRepresentationChange );

		if ( p_notify )
		{
			_notifyDataChanged();

			const int newRepresentationIndex = int( _representations.size() ) - 1;
			VTX_EVENT<int>( Event::Global::REPRESENTATION_ADDED, newRepresentationIndex );
		}
	};
	void RepresentationLibrary::copyRepresentation( const int p_index, const bool p_notify )
	{
		RepresentationPreset * const sourceRepresentation = _representations[ p_index ];
		RepresentationPreset * const copiedRepresentation = VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>(
			sourceRepresentation->getRepresentationType() );

		copiedRepresentation->copyDataFrom( *sourceRepresentation );

		copiedRepresentation->setName( getValidName( sourceRepresentation->getName() ) );

		addRepresentation( copiedRepresentation, p_notify );
	}
	RepresentationPreset * RepresentationLibrary::removeRepresentation( const int p_index, const bool p_notify )
	{
		RepresentationPreset * removedRepresentation;

		if ( 0 <= p_index && p_index < _representations.size() )
		{
			removedRepresentation = _representations[ p_index ];

			VTX::MVC_MANAGER().deleteView( _representations[ p_index ],
										   VTX::App::ID::View::REPRESENTATION_LIBRARY_ON_ITEMS );
			_representations.erase( _representations.begin() + p_index );

			if ( p_notify )
				_notifyDataChanged();

			VTX_EVENT<int>( Event::Global::REPRESENTATION_REMOVED, p_index );
		}
		else
		{
			removedRepresentation = nullptr;
		}

		return removedRepresentation;
	};

	bool RepresentationLibrary::canDeleteRepresentation( RepresentationPreset * const p_representation ) const
	{
		return p_representation != _defaultRepresentation;
	}

	void RepresentationLibrary::deleteRepresentation( const RepresentationPreset * const p_representation,
													  const bool						 p_notify )
	{
		const int representationIndex = getRepresentationIndex( p_representation );
		deleteRepresentation( representationIndex, p_notify );
	}

	void RepresentationLibrary::deleteRepresentation( const int p_index, const bool p_notify )
	{
		const RepresentationPreset * const representationToDelete = removeRepresentation( p_index, p_notify );

		if ( representationToDelete != nullptr )
			VTX::MVC_MANAGER().deleteModel( representationToDelete );
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
	RepresentationPreset * RepresentationLibrary::getDefaultRepresentation() { return _defaultRepresentation; }

	RepresentationPreset * RepresentationLibrary::getDefaultRepresentation(
		const Internal::ChemDB::Category::TYPE & p_categoryEnum )
	{
		const int representationIndex = VTX_SETTING().getDefaultRepresentationIndexPerCategory( p_categoryEnum );

		RepresentationPreset * res = getRepresentation( representationIndex );

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

		const std::string &	   defaultRepresentationName = VTX_SETTING().getTmpRepresentationDefaultName();
		RepresentationPreset * defaultRepresentation	 = getRepresentationByName( defaultRepresentationName );

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
		for ( const RepresentationPreset * const representation : _representations )
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
		REPRESENTATION_ENUM representationType;
		std::string			name;

		representationType = REPRESENTATION_ENUM::BALL_AND_STICK;
		name			   = "Ball And Stick";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const ballsAndsticks
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			ballsAndsticks->setName( name );
			ballsAndsticks->setQuickAccess( true );
			ballsAndsticks->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( ballsAndsticks, p_notify );
		}

		representationType = REPRESENTATION_ENUM::CARTOON;
		name			   = "Cartoon";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const cartoon
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			cartoon->setName( name );
			cartoon->setQuickAccess( true );
			cartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( cartoon, p_notify );
		}

		representationType = REPRESENTATION_ENUM::STICK_AND_CARTOON;
		name			   = "Cartoon + Stick";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const stickAndCartoon
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			stickAndCartoon->setName( name );
			stickAndCartoon->setQuickAccess( true );
			stickAndCartoon->getData().setCylinderRadius( 0.15f );
			stickAndCartoon->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( stickAndCartoon, p_notify );
		}

		representationType = REPRESENTATION_ENUM::SAS;
		name			   = "SAS";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const sas
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			sas->setName( name );
			sas->setQuickAccess( true );
			sas->getData().setColorMode( Generic::COLOR_MODE::CHAIN );
			addRepresentation( sas, p_notify );
		}

		representationType = REPRESENTATION_ENUM::SES;
		name			   = "SES";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const ses
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			ses->setName( name );
			ses->setQuickAccess( true );
			ses->getData().setColorMode( Generic::COLOR_MODE::PROTEIN );
			addRepresentation( ses, p_notify );
		}

		representationType = REPRESENTATION_ENUM::STICK;
		name			   = "Stick";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const stick
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset, REPRESENTATION_ENUM>( representationType );
			stick->setName( name );
			stick->setQuickAccess( true );
			stick->getData().setCylinderRadius( 0.15f );
			stick->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
			addRepresentation( stick, p_notify );
		}

		representationType = REPRESENTATION_ENUM::VAN_DER_WAALS;
		name			   = "VdW";
		if ( isExistingName( name ) == false )
		{
			RepresentationPreset * const vdw
				= VTX::MVC_MANAGER().instantiateModel<RepresentationPreset>( representationType );
			vdw->setName( name );
			vdw->setQuickAccess( true );
			vdw->getData().setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
			vdw->getData().setSphereRadius( 0 );
			addRepresentation( vdw, p_notify );
		}
	}

	void RepresentationLibrary::_onRepresentationChange( const Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
			_notifyViews( Event::Model::SUBITEM_DATA_CHANGE );
		else
			_notifyViews( new Core::Event::VTXEvent( *p_event ) );
	}

} // namespace VTX::App::Application::Representation
