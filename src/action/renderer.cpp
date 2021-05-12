#include "renderer.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/gl.hpp"
#include "ui/main_window.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/render_effect_saver.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::Action::Renderer
{
	void ReloadPresets::execute()
	{
		Worker::RenderEffectPresetLibraryLoader * libraryLoader
			= new Worker::RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary::get() );
		VTX_WORKER( libraryLoader );
	};

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::clearDirectory( Util::Filesystem::RENDER_EFFECT_PRESET_LIBRARY_DIR );
		}

		for ( const Model::Renderer::RenderEffectPreset * const renderEffect : _renderEffectPresets )
		{
			if ( _async )
			{
				Worker::RenderEffectPresetSaverThread * librarySaver
					= new Worker::RenderEffectPresetSaverThread( renderEffect );
				Worker::Callback * callback = new Worker::Callback( [ librarySaver ]( const uint p_code ) {} );

				VTX_WORKER( librarySaver, callback );
			}
			else
			{
				FilePath path = Util::Filesystem::getRenderEffectPath( renderEffect->getName() );
				Util::Filesystem::generateUniqueFileName( path );

				Worker::RenderEffectPresetSaver * librarySaver
					= new Worker::RenderEffectPresetSaver( renderEffect, path );
				VTX_WORKER( librarySaver );
			}
		}
	};

	void ApplyRenderEffectPreset::execute()
	{
		Model::Renderer::RenderEffectPresetLibrary::get().applyPreset( _preset );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeName::execute() { _preset.setName( _name ); };

	void ChangeQuickAccess::execute()
	{
		Model::Renderer::RenderEffectPresetLibrary::get().setQuickAccessToPreset( _preset, _quickAccess );
	};

	void ChangeShading ::execute()
	{
		_preset.setShading( _shading );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().setShading();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

	void EnableSSAO::execute()
	{
		_preset.enableSSAO( _enable );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeSSAO( _enable );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

	void ChangeSSAOIntensity::execute()
	{
		_preset.setSSAOIntensity( _intensity );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeSSAOBlurSize::execute()
	{
		_preset.setSSAOBlurSize( _blurSize );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void EnableOutline::execute()
	{
		_preset.enableOutline( _enable );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeOutline( _enable );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

	void ChangeOutlineThickness ::execute()
	{
		_preset.setOutlineThickness( _thickness );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeOutlineColor::execute()
	{
		_preset.setOutlineColor( _color );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void EnableFog::execute()
	{
		_preset.enableFog( _enable );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeFog( _enable );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

	void ChangeFogNear::execute()
	{
		_preset.setFogNear( _near );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeFogFar::execute()
	{
		_preset.setFogFar( _far );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeFogDensity::execute()
	{
		_preset.setFogDensity( _density );
		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeFogColor::execute()
	{
		_preset.setFogColor( _color );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};
	void ChangeBackgroundColor::execute()
	{
		_preset.setBackgroundColor( _color );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeBackgroundOpacity::execute()
	{
		_preset.setBackgroundOpacity( _opacity );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeCameraLightColor::execute()
	{
		_preset.setCameraLightColor( _color );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void ChangeCameraFOV::execute()
	{
		_preset.setCameraFOV( _fov );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}
	};

	void ChangeCameraNear::execute()
	{
		_preset.setCameraNearClip( _near );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}
	};

	void ChangeCameraFar::execute()
	{
		_preset.setCameraFarClip( _far );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}
	};

	void EnableCameraAntialiasing::execute()
	{
		_preset.setAA( _enable );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeAA( _enable );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

	void ChangeCameraPerspective::execute()
	{
		_preset.setPerspectiveProjection( _perspective );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}
	};

	void AddNewPresetInLibrary::execute()
	{
		Model::Renderer::RenderEffectPreset * const newRenderEffect
			= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

		newRenderEffect->setName( _presetName );
		Model::Renderer::RenderEffectPresetLibrary::get().addPreset( newRenderEffect );
	};

	void CopyPresetInLibrary::execute()
	{
		Model::Renderer::RenderEffectPresetLibrary::get().copyPreset( _presetIndex );
	};

	void DeletePresetInLibrary::execute()
	{
		Model::Renderer::RenderEffectPresetLibrary::get().deletePreset( _presetIndex );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};
} // namespace VTX::Action::Renderer
