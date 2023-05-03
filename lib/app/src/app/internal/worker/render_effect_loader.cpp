#include "app/internal/worker/render_effect_loader.hpp"
#include "app/application/render_effect/render_effect_library.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include <filesystem>
#include <util/logger.hpp>

namespace VTX::App::Internal::Worker
{
	void RenderEffectPresetLibraryLoader::_run()
	{
		Util::Chrono chrono;

		App::Core::IO::Reader::SerializedObject<App::Application::RenderEffect::RenderEffectPreset> * const reader
			= new App::Core::IO::Reader::SerializedObject<App::Application::RenderEffect::RenderEffectPreset>();

		chrono.start();

		_library.clear( false );

		if ( std::filesystem::exists( _path ) )
		{
			for ( const std::filesystem::directory_entry & file : std::filesystem::directory_iterator { _path } )
			{
				App::Application::RenderEffect::RenderEffectPreset * const preset
					= VTX::MVC_MANAGER().instantiateModel<App::Application::RenderEffect::RenderEffectPreset>();

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
					VTX::MVC_MANAGER().deleteModel( preset );
				}
			}

			VTX_INFO( "Render effect library loaded." );
		}
		else
		{
			VTX_WARNING( "Can't find render effect library folder at {}. Load Default.", _path.string() );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void RenderEffectPresetLoader::_run()
	{
		Util::Chrono chrono;

		App::Core::IO::Reader::SerializedObject<App::Application::RenderEffect::RenderEffectPreset> * const reader
			= new App::Core::IO::Reader::SerializedObject<App::Application::RenderEffect::RenderEffectPreset>();

		chrono.start();

		for ( const FilePath & path : _paths )
		{
			App::Application::RenderEffect::RenderEffectPreset * const preset
				= VTX::MVC_MANAGER().instantiateModel<App::Application::RenderEffect::RenderEffectPreset>();

			try
			{
				reader->readFile( path, *preset );
				preset->setName( path.stem().string() );
				App::Application::RenderEffect::RenderEffectLibrary::get().addPreset( preset, true, true );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Cannot load render effect preset at " + path.string() + " : " + std::string( p_e.what() ) );
				VTX::MVC_MANAGER().deleteModel( preset );
			}

			VTX_INFO( "Render effect preset " + path.stem().string() + " loaded." );
		}

		delete reader;

		chrono.stop();

		VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
} // namespace VTX::App::Internal::Worker
