#include "app/old/internal/worker/render_effect_saver.hpp"
#include "app/old/application/render_effect/render_effect_preset.hpp"
#include "app/old/core/io/writer/serialized_object.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/mvc.hpp"
#include <exception>
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Worker
{
	uint RenderEffectPresetSaverThread::_run()
	{
		bool		 result = 1;
		Util::Chrono chrono;

		chrono.start();
		emitLogInfo( "Saving " + _preset->getName() );
		const FilePath path = App::Old::Internal::IO::Filesystem::getRenderEffectPath( _preset->getName() );

		App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::RenderEffect::RenderEffectPreset> * writer
			= new App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::RenderEffect::RenderEffectPreset>();

		// Write.
		try
		{
			writer->writeFile( path, *_preset );
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
	void RenderEffectPresetSaver::_run()
	{
		Util::Chrono chrono;

		chrono.start();
		VTX_INFO( "Saving {}...", _preset->getName() );

		App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::RenderEffect::RenderEffectPreset> * writer
			= new App::Old::Core::IO::Writer::SerializedObject<App::Old::Application::RenderEffect::RenderEffectPreset>();

		// Write.
		try
		{
			writer->writeFile( _path, *_preset );
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
