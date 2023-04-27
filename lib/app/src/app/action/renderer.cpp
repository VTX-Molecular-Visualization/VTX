#include "app/action/renderer.hpp"
#include "app/internal/worker/render_effect_loader.hpp"
#include "app/internal/worker/render_effect_saver.hpp"
#include "app/mvc.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/object3d/camera.hpp"
#include "app/old_app/renderer/gl/gl.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker.hpp"

namespace VTX::App::Action::Renderer
{
	void ReloadPresets::execute()
	{
		Worker::RenderEffectPresetLibraryLoader * libraryLoader
			= new Worker::RenderEffectPresetLibraryLoader( App::Application::RenderEffect::RenderEffectLibrary::get() );

		VTX::App::Core::Worker::CallbackWorker * const callback = new VTX::App::Core::Worker::CallbackWorker(
			[]()
			{
				App::Application::RenderEffect::RenderEffectLibrary::get().applyPreset(
					VTX_SETTING().getDefaultRenderEffectPresetIndex() );
			} );

		VTX_WORKER( libraryLoader, callback );
	}

	void ResetPresets::execute() { App::Application::RenderEffect::RenderEffectLibrary::get().resetToDefault(); }

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::removeAll( IO::Filesystem::getRenderEffectPresetsLibraryDir() );
			std::filesystem::create_directory( IO::Filesystem::getRenderEffectPresetsLibraryDir() );
		}

		for ( const App::Application::RenderEffect::RenderEffectPreset * const renderEffect : _renderEffectPresets )
		{
			if ( _async )
			{
				Worker::RenderEffectPresetSaverThread * librarySaver
					= new Worker::RenderEffectPresetSaverThread( renderEffect );
				VTX::App::Core::Worker::CallbackThread * callback
					= new VTX::App::Core::Worker::CallbackThread( []( const uint p_code ) {} );

				VTX_THREAD( librarySaver, callback );
			}
			else
			{
				FilePath path = IO::Filesystem::getRenderEffectPath( renderEffect->getName() );
				Util::Filesystem::generateUniqueFileName( path );

				Worker::RenderEffectPresetSaver * librarySaver
					= new Worker::RenderEffectPresetSaver( renderEffect, path );
				VTX_WORKER( librarySaver );
			}
		}
	}

	void ApplyRenderEffectPreset::execute()
	{
		App::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( _preset );

		if ( _setAsDefault )
		{
			const int presetIndex = App::Application::RenderEffect::RenderEffectLibrary::get().getPresetIndex( &_preset );

			if ( presetIndex >= 0 )
				VTX_SETTING().setDefaultRenderEffectPresetIndex( presetIndex );
		}

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeName::execute() { _preset.setName( _name ); }

	void ChangeQuickAccess::execute()
	{
		App::Application::RenderEffect::RenderEffectLibrary::get().setQuickAccessToPreset( _preset, _quickAccess );
	}

	void ChangeShading::execute()
	{
		_preset.setShading( _shading );

		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	}

	void EnableSSAO::execute()
	{
		_preset.enableSSAO( _enable );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeSSAOIntensity::execute()
	{
		_preset.setSSAOIntensity( _intensity );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeSSAOBlurSize::execute()
	{
		_preset.setSSAOBlurSize( _blurSize );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void EnableOutline::execute()
	{
		_preset.enableOutline( _enable );

		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeOutlineThickness ::execute()
	{
		_preset.setOutlineThickness( _thickness );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeOutlineSensivity ::execute()
	{
		_preset.setOutlineSensivity( _sensivity );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeOutlineColor::execute()
	{
		_preset.setOutlineColor( _color );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void EnableFog::execute()
	{
		_preset.enableFog( _enable );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	}

	void ChangeFogNear::execute()
	{
		_preset.setFogNear( _near );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogFar::execute()
	{
		_preset.setFogFar( _far );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogDensity::execute()
	{
		_preset.setFogDensity( _density );
		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void ChangeFogColor::execute()
	{
		_preset.setFogColor( _color );

		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}
	void ChangeBackgroundColor::execute()
	{
		_preset.setBackgroundColor( _color );

		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}
	void ChangeCameraLightColor::execute()
	{
		_preset.setCameraLightColor( _color );

		if ( App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	}

	void AddNewPresetInLibrary::execute()
	{
		App::Application::RenderEffect::RenderEffectPreset * const newRenderEffect
			= VTX::MVC_MANAGER().instantiateModel<App::Application::RenderEffect::RenderEffectPreset>();

		newRenderEffect->setName( _presetName );
		App::Application::RenderEffect::RenderEffectLibrary::get().addPreset( newRenderEffect );
		App::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( *newRenderEffect );
	}

	void CopyPresetInLibrary::execute()
	{
		App::Application::RenderEffect::RenderEffectPreset * const newPreset
			= App::Application::RenderEffect::RenderEffectLibrary::get().copyPreset( _presetIndex );
		App::Application::RenderEffect::RenderEffectLibrary::get().applyPreset( *newPreset );
	}

	void DeletePresetInLibrary::execute()
	{
		App::Application::RenderEffect::RenderEffectLibrary::get().deletePreset( _presetIndex );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}
} // namespace VTX::App::Action::Renderer
