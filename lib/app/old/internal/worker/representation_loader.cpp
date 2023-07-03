#include "app/old/internal/worker/representation_loader.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/core/io/reader/serialized_object.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <filesystem>
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Worker
{
	void RepresentationLibraryLoader::_run()
	{
		Util::Chrono chrono;

		App::Old::Core::IO::Reader::SerializedObject<App::Old::Application::Representation::RepresentationPreset> * const reader
			= new App::Old::Core::IO::Reader::SerializedObject<App::Old::Application::Representation::RepresentationPreset>();

		chrono.start();

		if ( _restore )
		{
			App::Old::Application::Representation::RepresentationManager::get().storeRepresentations();
			App::Old::Application::Representation::RepresentationManager::get().clearAllRepresentations( false );
		}

		if ( std::filesystem::exists( _path ) )
		{
			for ( const std::filesystem::directory_entry & file : std::filesystem::directory_iterator { _path } )
			{
				App::Old::Application::Representation::RepresentationPreset * const representation
					= VTX::MVC_MANAGER().instantiateModel<App::Old::Application::Representation::RepresentationPreset>();

				try
				{
					reader->readFile( file, *representation );
					representation->setName( file.path().stem().string() );
					_library.addRepresentation( representation, false );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Cannot load representation library {} : {}", file.path().string(), p_e.what() );
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

		if ( App::Old::VTXApp::get().getSetting().getTmpRepresentationDefaultName() != "" )
		{
			App::Old::Application::Representation::RepresentationPreset * defaultRepresentation
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
			App::Old::Application::Representation::RepresentationPreset * defaultRepresentation
				= _library.getRepresentationByName( VTX_SETTING().REPRESENTATION_DEFAULT_NAME );

			const int defaultRepresentationIndex = _library.getRepresentationIndex( defaultRepresentation );

			_library.setDefaultRepresentation( defaultRepresentationIndex, false );
		}

		for ( int i = 0; i < int( VTX::Core::ChemDB::Category::TYPE::COUNT ); i++ )
		{
			const VTX::Core::ChemDB::Category::TYPE categoryEnum = VTX::Core::ChemDB::Category::TYPE( i );
			const std::string &						representationName
				= App::Old::VTXApp::get().getSetting().getTmpDefaultRepresentationNamePerCategory( categoryEnum );

			if ( representationName.empty() )
				continue;

			App::Old::Application::Representation::RepresentationPreset * defaultRepresentation
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
			App::Old::Application::Representation::RepresentationManager::get().restoreRepresentations();

		if ( _notify )
			_library.forceNotifyDataChanged();

		chrono.stop();

		VTX_INFO( "File treated in {}.", chrono.elapsedTimeStr() );
	}

	void RepresentationLoader::_run()
	{
		Util::Chrono chrono;

		App::Old::Core::IO::Reader::SerializedObject<App::Old::Application::Representation::RepresentationPreset> * const reader
			= new App::Old::Core::IO::Reader::SerializedObject<App::Old::Application::Representation::RepresentationPreset>();

		chrono.start();

		for ( const FilePath & path : _paths )
		{
			App::Old::Application::Representation::RepresentationPreset * const representation
				= VTX::MVC_MANAGER().instantiateModel<App::Old::Application::Representation::RepresentationPreset>();

			try
			{
				reader->readFile( path, *representation );
				representation->setName( path.stem().string() );
				App::Old::Application::Representation::RepresentationLibrary::get().addRepresentation( representation,
																								  true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load representation at {} : {}", path.string(), p_e.what() );
				VTX::MVC_MANAGER().deleteModel( representation );
			}

			VTX_INFO( "Representation {} loaded.", path.stem().string() );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in {}.", chrono.elapsedTimeStr() );
	}
} // namespace VTX::App::Old::Internal::Worker
