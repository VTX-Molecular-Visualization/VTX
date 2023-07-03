#include "app/old/action/renderer.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/internal/worker/render_effect_loader.hpp"
#include "app/old/internal/worker/render_effect_saver.hpp"
#include "app/old/mvc.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/render/renderer/gl/gl.hpp"
#include "app/old/worker.hpp"

namespace VTX::App::Old::Action::Renderer
{
	void ReloadPresets::execute()
	{
		Internal::Worker::RenderEffectPresetLibraryLoader * libraryLoader
			= new Internal::Worker::RenderEffectPresetLibraryLoader(
				App::Old::Application::RenderEffect::RenderEffectLibrary::get() );

		VTX::App::Old::Core::Worker::CallbackWorker * const callback = new VTX::App::Old::Core::Worker::CallbackWorker(
			[]()
			{
				App::Old::Application::RenderEffect::RenderEffectLibrary::get().applyPreset(
					VTX_SETTING().getDefaultRenderEffectPresetIndex() );
			} );

		VTX_WORKER( libraryLoader, callback );
	}

	void ResetPresets::execute() { App::Old::Application::RenderEffect::RenderEffectLibrary::get().resetToDefault(); }

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::removeAll( App::Old::Internal::IO::Filesystem::getRenderEffectPresetsLibraryDir() );
			std::filesystem::create_directory( App::Old::Internal::IO::Filesystem::getRenderEffectPresetsLibraryDir() );
		}

		for ( const App::Old::Application::RenderEffect::RenderEffectPreset * const renderEffect : _renderEffectPresets )
		{
			if ( _async )
			{
				Internal::Worker::RenderEffectPresetSaverThread * librarySaver
					= new Internal::Worker::RenderEffectPresetSaverThread( renderEffect );
				VTX::App::Old::Core::Worker::CallbackThread * callback
					= new VTX::App::Old::Core::Worker::CallbackThread( []( const uint p_code ) {} );

				VTX_THREAD( librarySaver, callback );
			}
			else
			{
				FilePath path = App::Old::Internal::IO::Filesystem::getRenderEffectPath( renderEffect->getName() );
				Util::Filesystem::generateUniqueFileName( path );

				Internal::Worker::RenderEffectPresetSaver * librarySaver
					= new Internal::Worker::RenderEffectPresetSaver( renderEffect, path );
				VTX_WORKER( librarySaver );
			}
		}
	}

	void ApplyRenderEffectPreset::execute()
	{
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( _preset );

		if ( _setAsDefault )
		{
			const int presetIndex
				= App::Old::Application::RenderEffect::RenderEffectLibrary::get().getPresetIndex( &_preset );

			if ( presetIndex >= 0 )
				VTX_SETTING().setDefaultRenderEffectPresetIndex( presetIndex );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

	void ChangeName::execute() { _preset.setName( _name ); }

	void ChangeQuickAccess::execute()
	{
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().setQuickAccessToPreset( _preset, _quickAccess );
	}

	void ChangeShading::execute()
	{
		_preset.setShading( _shading );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
		}
	}

	void EnableSSAO::execute()
	{
		_preset.enableSSAO( _enable );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeSSAOIntensity::execute()
	{
		_preset.setSSAOIntensity( _intensity );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeSSAOBlurSize::execute()
	{
		_preset.setSSAOBlurSize( _blurSize );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void EnableOutline::execute()
	{
		_preset.enableOutline( _enable );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeOutlineThickness ::execute()
	{
		_preset.setOutlineThickness( _thickness );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeOutlineSensivity ::execute()
	{
		_preset.setOutlineSensivity( _sensivity );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeOutlineColor::execute()
	{
		_preset.setOutlineColor( _color );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void EnableFog::execute()
	{
		_preset.enableFog( _enable );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeFogNear::execute()
	{
		_preset.setFogNear( _near );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogFar::execute()
	{
		_preset.setFogFar( _far );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogDensity::execute()
	{
		_preset.setFogDensity( _density );
		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogColor::execute()
	{
		_preset.setFogColor( _color );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}
	void ChangeBackgroundColor::execute()
	{
		_preset.setBackgroundColor( _color );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}
	void ChangeCameraLightColor::execute()
	{
		_preset.setCameraLightColor( _color );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void AddNewPresetInLibrary::execute()
	{
		App::Old::Application::RenderEffect::RenderEffectPreset * const newRenderEffect
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Application::RenderEffect::RenderEffectPreset>();

		newRenderEffect->setName( _presetName );
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().addPreset( newRenderEffect );
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( *newRenderEffect );
	}

	void CopyPresetInLibrary::execute()
	{
		App::Old::Application::RenderEffect::RenderEffectPreset * const newPreset
			= App::Old::Application::RenderEffect::RenderEffectLibrary::get().copyPreset( _presetIndex );
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( *newPreset );
	}

	void DeletePresetInLibrary::execute()
	{
		App::Old::Application::RenderEffect::RenderEffectLibrary::get().deletePreset( _presetIndex );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}
} // namespace VTX::App::Old::Action::Renderer
