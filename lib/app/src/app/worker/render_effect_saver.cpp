#include "app/worker/render_effect_saver.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/io/writer/serialized_object.hpp"
#include "app/old_app/model/renderer/render_effect_preset.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include <exception>
#include <util/logger.hpp>

namespace VTX::Worker
{
	uint RenderEffectPresetSaverThread::_run()
	{
		bool		 result = 1;
		Util::Chrono chrono;

		chrono.start();
		emitLogInfo( "Saving " + _preset->getName() );
		const FilePath path = IO::Filesystem::getRenderEffectPath( _preset->getName() );

		IO::Writer::SerializedObject<Model::Renderer::RenderEffectPreset> * writer
			= new IO::Writer::SerializedObject<Model::Renderer::RenderEffectPreset>();

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
		VTX_INFO( "Saving " + _preset->getName() );

		IO::Writer::SerializedObject<Model::Renderer::RenderEffectPreset> * writer
			= new IO::Writer::SerializedObject<Model::Renderer::RenderEffectPreset>();

		// Write.
		try
		{
			writer->writeFile( _path, *_preset );
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
