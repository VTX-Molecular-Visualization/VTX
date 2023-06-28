#include "app/old/internal/worker/representation_saver.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/core/io/writer/serialized_object.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/mvc.hpp"
#include <exception>
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Worker
{
	/*
	uint RepresentationSaverThread::_run()
	{
		bool		 result = 1;
		Util::Chrono chrono;

		chrono.start();
		emit		   logInfo( "Saving " + _representation->getName() );
		const FilePath path = App::Old::Internal::IO::Filesystem::getRepresentationPath( _representation->getName() );

		Core::IO::Writer::SerializedObject<App::Old::Application::Representation::RepresentationPreset> * writer
			= new Core::IO::Writer::SerializedObject<App::Old::Application::Representation::RepresentationPreset>();

		// Write.
		try
		{
			writer->writeFile( path.string(), *_representation );
		}
		catch ( const std::exception & p_e )
		{
			emitLogError( "Error saving file" );
			emitLogError( p_e.what() );
			result = 0;
		}

		delete writer;

		chrono.stop();
		emitLogInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

		return result;
	}
	*/

	void RepresentationSaver::_run()
	{
		Util::Chrono chrono;

		chrono.start();
		VTX_INFO( "Saving {}", _representation->getName() );

		App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::Representation::RepresentationPreset> * writer
			= new App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::Representation::RepresentationPreset>();

		// Write.
		try
		{
			writer->writeFile( _path, *_representation );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error saving file" );
			VTX_ERROR( "{}", p_e.what() );
		}

		delete writer;

		chrono.stop();
		VTX_INFO( "File treated in {}.", chrono.elapsedTimeStr() );
	}

} // namespace VTX::App::Old::Internal::Worker
