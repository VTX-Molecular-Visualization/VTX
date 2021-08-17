#include "render_effect_loader.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX::Worker
{
	void RenderEffectPresetLibraryLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset> * const reader
			= new IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset>();

		chrono.start();

		_library.clear( false );

		std::set<IO::FilePath> files = Util::Filesystem::getFilesInDirectory( _path );

		for ( const IO::FilePath & file : files )
		{
			Model::Renderer::RenderEffectPreset * const preset
				= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

			try
			{
				reader->readFile( file, *preset );
				preset->setName( file.filenameWithoutExtension() );
				_library.addPreset( preset, true, _notify );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load render effect library " + file.path() + ": " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( preset );
			}
		}

		VTX_INFO( "Render effect library loaded." );

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void RenderEffectPresetLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset> * const reader
			= new IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset>();

		chrono.start();

		for ( const IO::FilePath & path : _paths )
		{
			Model::Renderer::RenderEffectPreset * const preset
				= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

			try
			{
				reader->readFile( path, *preset );
				Model::Renderer::RenderEffectPresetLibrary::get().addPreset( preset, true, true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load render effect preset at " + path.path() + " : " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( preset );
			}

			VTX_INFO( "Render effect preset " + path.filenameWithoutExtension() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::Worker
