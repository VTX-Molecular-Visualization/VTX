#include "app/internal/worker/representation_loader.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/internal/io/filesystem.hpp"
#include <filesystem>
#include <util/logger.hpp>

namespace VTX::App::Internal::Worker
{
	void RepresentationLibraryLoader::_run()
	{
		Util::Chrono chrono;

		App::Core::IO::Reader::SerializedObject<App::Application::Representation::RepresentationPreset> * const reader
			= new App::Core::IO::Reader::SerializedObject<App::Application::Representation::RepresentationPreset>();

		chrono.start();

		if ( _restore )
		{
			App::Application::Representation::RepresentationManager::get().storeRepresentations();
			App::Application::Representation::RepresentationManager::get().clearAllRepresentations( false );
		}

		if ( std::filesystem::exists( _path ) )
		{
			for ( const std::filesystem::directory_entry & file : std::filesystem::directory_iterator { _path } )
			{
				App::Application::Representation::RepresentationPreset * const representation
					= VTX::MVC_MANAGER().instantiateModel<App::Application::Representation::RepresentationPreset>();

				try
				{
					reader->readFile( file, *representation );
					representation->setName( file.path().stem().string() );
					_library.addRepresentation( representation, false );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR(
						"Cannot load representation library {}: {}", file.path().string(), std::string( p_e.what() ) );
					VTX::MVC_MANAGER().deleteModel( representation );
				}
			}

			VTX_INFO( "Representation library loaded." );
		}
		else
		{
			VTX_WARNING( "Can't find representation library folder at {}. Load Default.", _path.string() );
		}

		delete reader;

		_library.applyDefault( _notify );

		if ( VTXApp::get().getSetting().getTmpRepresentationDefaultName() != "" )
		{
			App::Application::Representation::RepresentationPreset * defaultRepresentation
				= _library.getRepresentationByName( VTX_SETTING().getTmpRepresentationDefaultName() );

			if ( defaultRepresentation == nullptr )
			{
				defaultRepresentation = _library.getRepresentationByName( VTX_SETTING().REPRESENTATION_DEFAULT_NAME );

				if ( defaultRepresentation == nullptr )
					defaultRepresentation = _library.getRepresentation( 0 );
			}

			const int defaultRepresentationIndex = _library.getRepresentationIndex( defaultRepresentation );

			VTX_SETTING().setDefaultRepresentationIndex( defaultRepresentationIndex );
			_library.setDefaultRepresentation( defaultRepresentationIndex, false );
		}
		else
		{
			App::Application::Representation::RepresentationPreset * defaultRepresentation
				= _library.getRepresentationByName( VTX_SETTING().REPRESENTATION_DEFAULT_NAME );

			const int defaultRepresentationIndex = _library.getRepresentationIndex( defaultRepresentation );

			_library.setDefaultRepresentation( defaultRepresentationIndex, false );
		}

		for ( int i = 0; i < int( App::Internal::ChemDB::Category::TYPE::COUNT ); i++ )
		{
			const App::Internal::ChemDB::Category::TYPE categoryEnum = App::Internal::ChemDB::Category::TYPE( i );
			const std::string &							representationName
				= VTXApp::get().getSetting().getTmpDefaultRepresentationNamePerCategory( categoryEnum );

			if ( representationName.empty() )
				continue;

			App::Application::Representation::RepresentationPreset * defaultRepresentation
				= _library.getRepresentationByName( representationName );

			if ( defaultRepresentation == nullptr )
			{
				defaultRepresentation
					= _library.getRepresentationByName( VTX_SETTING().DEFAULT_REPRESENTATION_PER_CATEGORY_NAME[ i ] );

				if ( defaultRepresentation == nullptr )
					defaultRepresentation = _library.getRepresentation( 0 );
			}

			const int defaultRepresentationIndex = _library.getRepresentationIndex( defaultRepresentation );

			VTX_SETTING().setDefaultRepresentationIndexPerCategory( categoryEnum, defaultRepresentationIndex );
		}

		if ( _restore )
			App::Application::Representation::RepresentationManager::get().restoreRepresentations();

		if ( _notify )
			_library.forceNotifyDataChanged();

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void RepresentationLoader::_run()
	{
		Util::Chrono chrono;

		App::Core::IO::Reader::SerializedObject<App::Application::Representation::RepresentationPreset> * const reader
			= new App::Core::IO::Reader::SerializedObject<App::Application::Representation::RepresentationPreset>();

		chrono.start();

		for ( const FilePath & path : _paths )
		{
			App::Application::Representation::RepresentationPreset * const representation
				= VTX::MVC_MANAGER().instantiateModel<App::Application::Representation::RepresentationPreset>();

			try
			{
				reader->readFile( path, *representation );
				representation->setName( path.stem().string() );
				App::Application::Representation::RepresentationLibrary::get().addRepresentation( representation,
																								  true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load representation at {}: {}", path.string(), std::string( p_e.what() ) );
				VTX::MVC_MANAGER().deleteModel( representation );
			}

			VTX_INFO( "Representation " + path.stem().string() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::App::Internal::Worker
