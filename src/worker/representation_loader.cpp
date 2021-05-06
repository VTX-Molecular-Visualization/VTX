#include "representation_loader.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/representation/representation_library.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include <filesystem>

namespace VTX::Worker
{
	void RepresentationLibraryLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Representation::Representation> * const reader
			= new IO::Reader::SerializedObject<Model::Representation::Representation>();

		chrono.start();

		_library.clear( false );

		std::filesystem::directory_iterator fileIterator = std::filesystem::directory_iterator( *_path );

		while ( !fileIterator._At_end() )
		{
			if ( fileIterator->is_regular_file() )
			{
				Model::Representation::Representation * const representation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::Representation>();

				try
				{
					reader->readFile( fileIterator->path(), *representation );
					_library.addRepresentation( representation, false );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Cannot load representation library : " + std::string( p_e.what() ) );
					MVC::MvcManager::get().deleteModel( representation );
				}
			}

			fileIterator++;
		}

		VTX_INFO( "Representation library loaded." );

		delete reader;

		if ( _notify )
			_library.forceNotifyDataChanged();

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::Worker
