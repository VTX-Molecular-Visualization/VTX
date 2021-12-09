#include "representation_loader.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/representation/representation_library.hpp"
#include "representation/representation_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"

namespace VTX::Worker
{
	void RepresentationLibraryLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Representation::Representation> * const reader
			= new IO::Reader::SerializedObject<Model::Representation::Representation>();

		chrono.start();

		if ( _restore )
		{
			Representation::RepresentationManager::get().storeRepresentations();
			Representation::RepresentationManager::get().clearAllRepresentations( false );
		}

		std::set<IO::FilePath> files = Util::Filesystem::getFilesInDirectory( _path );

		for ( const IO::FilePath & file : files )
		{
			Model::Representation::Representation * const representation
				= MVC::MvcManager::get().instantiateModel<Model::Representation::Representation>();

			try
			{
				reader->readFile( file, *representation );
				representation->setName( file.filenameWithoutExtension() );
				_library.addRepresentation( representation, false );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load representation library " + file.path() + ": " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( representation );
			}
		}

		VTX_INFO( "Representation library loaded." );

		delete reader;

		if ( _library.getRepresentationCount() <= 0 )
		{
			_library.applyDefault( _notify );
		}

		if ( VTXApp::get().getSetting().getTmpRepresentationDefaultName() != "" )
		{
			Model::Representation::Representation * defaultRepresentation
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
			_library.setDefaultRepresentation( VTX_SETTING().getDefaultRepresentationIndex(), false );
		}

		if ( _restore )
			Representation::RepresentationManager::get().restoreRepresentations();

		if ( _notify )
			_library.forceNotifyDataChanged();

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void RepresentationLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Representation::Representation> * const reader
			= new IO::Reader::SerializedObject<Model::Representation::Representation>();

		chrono.start();

		for ( const IO::FilePath & path : _paths )
		{
			Model::Representation::Representation * const representation
				= MVC::MvcManager::get().instantiateModel<Model::Representation::Representation>();

			try
			{
				reader->readFile( path, *representation );
				representation->setName( path.filenameWithoutExtension() );
				Model::Representation::RepresentationLibrary::get().addRepresentation( representation, true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load representation at " + path.path() + " : " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( representation );
			}

			VTX_INFO( "Representation " + path.filenameWithoutExtension() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::Worker
