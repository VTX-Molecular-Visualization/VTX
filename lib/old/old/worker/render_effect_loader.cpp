#include "render_effect_loader.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Worker
{
	void RenderEffectPresetLibraryLoader::_run()
	{
		Util::Chrono chrono;

		IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset> * const reader
			= new IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset>();

		chrono.start();

		_library.clear( false );

		for ( const std::filesystem::directory_entry & file : std::filesystem::directory_iterator { _path } )
		{
			Model::Renderer::RenderEffectPreset * const preset
				= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

			try
			{
				reader->readFile( file, *preset );
				preset->setName( file.path().stem().string() );
				_library.addPreset( preset, true, _notify );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load render effect library " + file.path().string() + ": "
						   + std::string( p_e.what() ) );
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
		Util::Chrono chrono;

		IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset> * const reader
			= new IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset>();

		chrono.start();

		for ( const FilePath & path : _paths )
		{
			Model::Renderer::RenderEffectPreset * const preset
				= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

			try
			{
				reader->readFile( path, *preset );
				preset->setName( path.stem().string() );
				Model::Renderer::RenderEffectPresetLibrary::get().addPreset( preset, true, true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load render effect preset at " + path.string() + " : " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( preset );
			}

			VTX_INFO( "Render effect preset " + path.stem().string() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::Worker
