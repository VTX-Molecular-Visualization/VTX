#include "representation_saver.hpp"
#include "io/filesystem.hpp"
#include "io/writer/serialized_object.hpp"
#include "model/representation/representation.hpp"
#include "mvc/mvc_manager.hpp"
#include <exception>
#include <util/logger.hpp>

namespace VTX::Worker
{
	/*
	uint RepresentationSaverThread::_run()
	{
		bool		 result = 1;
		Util::Chrono chrono;

		chrono.start();
		emit		   logInfo( "Saving " + _representation->getName() );
		const FilePath path = IO::Filesystem::getRepresentationPath( _representation->getName() );

		IO::Writer::SerializedObject<Model::Representation::Representation> * writer
			= new IO::Writer::SerializedObject<Model::Representation::Representation>();

		// Write.
		try
		{
			writer->writeFile( path.string(), *_representation );
		}
		catch ( const std::exception & p_e )
		{
			emit logError( "Error saving file" );
			emit logError( p_e.what() );
			result = 0;
		}

		delete writer;

		chrono.stop();
		emit logInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

		return result;
	}
	*/

	void RepresentationSaver::_run()
	{
		Util::Chrono chrono;

		chrono.start();
		VTX_INFO( "Saving " + _representation->getName() );

		IO::Writer::SerializedObject<Model::Representation::Representation> * writer
			= new IO::Writer::SerializedObject<Model::Representation::Representation>();

		// Write.
		try
		{
			writer->writeFile( _path, *_representation );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error saving file" );
			VTX_ERROR( p_e.what() );
		}

		delete writer;

		chrono.stop();
		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

} // namespace VTX::Worker
