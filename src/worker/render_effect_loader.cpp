#include "render_effect_loader.hpp"
#include "io/reader/serialized_object.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include <filesystem>

namespace VTX::Worker
{
	void RenderEffectPresetLibraryLoader::_run()
	{
		Tool::Chrono chrono;

		IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset> * const reader
			= new IO::Reader::SerializedObject<Model::Renderer::RenderEffectPreset>();

		chrono.start();

		_library.clear( false );

		std::filesystem::directory_iterator fileIterator = std::filesystem::directory_iterator( _path );

		while ( !fileIterator._At_end() )
		{
			if ( fileIterator->is_regular_file() )
			{
				Model::Renderer::RenderEffectPreset * const preset
					= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

				try
				{
					reader->readFile( fileIterator->path(), *preset );
					preset->setName( fileIterator->path().filename().string() );
					_library.addPreset( preset, true, _notify );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Cannot load render effect library : " + std::string( p_e.what() ) );
					MVC::MvcManager::get().deleteModel( preset );
				}
			}

			fileIterator++;
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

		for ( const FilePath & path : _paths )
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
				VTX_ERROR( "Cannot load render effect preset at " + path.string() + " : " + std::string( p_e.what() ) );
				MVC::MvcManager::get().deleteModel( preset );
			}

			VTX_INFO( "render effect preset " + path.filename().string() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::Worker
