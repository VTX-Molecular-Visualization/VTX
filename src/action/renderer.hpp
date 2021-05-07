#ifndef __VTX_ACTION_RENDERER__
#define __VTX_ACTION_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "color/rgb.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/base_renderer.hpp"
#include "vtx_app.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/render_effect_saver.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::Action::Renderer
{
	class ReloadPresets : public BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override
		{
			Worker::RenderEffectPresetLibraryLoader * libraryLoader
				= new Worker::RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary::get() );
			VTX_WORKER( libraryLoader );
		};
	};

	class SavePreset : public BaseAction
	{
	  public:
		SavePreset( const Model::Renderer::RenderEffectPreset & p_representation )
		{
			_renderEffectPresets.emplace( &p_representation );
		};
		SavePreset( const std::unordered_set<const Model::Renderer::RenderEffectPreset *> & p_representations )
		{
			for ( const Model::Renderer::RenderEffectPreset * const representation : p_representations )
				_renderEffectPresets.emplace( representation );
		};
		SavePreset( Model::Renderer::RenderEffectPresetLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getPresetCount(); i++ )
				_renderEffectPresets.emplace( p_library.getPreset( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override
		{
			if ( _clearDirectory )
			{
				Util::Filesystem::clearDirectory( Util::Filesystem::REPRESENTATION_LIBRARY_DIR );
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
					Worker::RenderEffectPresetSaver * librarySaver
						= new Worker::RenderEffectPresetSaver( renderEffect );
					VTX_WORKER( librarySaver );
				}
			}
		};

	  private:
		std::unordered_set<const Model::Renderer::RenderEffectPreset *> _renderEffectPresets
			= std::unordered_set<const Model::Renderer::RenderEffectPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ApplyRenderEffectPreset : public BaseAction
	{
	  public:
		ApplyRenderEffectPreset( const Model::Renderer::RenderEffectPreset & p_preset ) : _preset( p_preset ) {};
		virtual void execute() override
		{
			Model::Renderer::RenderEffectPresetLibrary::get().applyPreset( _preset );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const Model::Renderer::RenderEffectPreset & _preset;
	};

	class ChangeName : public BaseAction
	{
	  public:
		ChangeName( Model::Renderer::RenderEffectPreset & p_preset, const std::string & p_name ) :
			_preset( p_preset ), _name( p_name ) {};

		virtual void execute() override { _preset.setName( _name ); };

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const std::string					  _name;
	};

	class ChangeQuickAccess : public BaseAction
	{
	  public:
		ChangeQuickAccess( Model::Renderer::RenderEffectPreset & p_preset, const bool p_quickAccess ) :
			_preset( p_preset ), _quickAccess( p_quickAccess ) {};

		virtual void execute() override
		{
			Model::Renderer::RenderEffectPresetLibrary::get().setQuickAccessToPreset( _preset, _quickAccess );
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _quickAccess;
	};

	class ChangeShading : public BaseAction
	{
	  public:
		ChangeShading( Model::Renderer::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override
		{
			_preset.setShading( _shading );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public BaseAction
	{
	  public:
		EnableSSAO( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableSSAO( _enable );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeSSAOIntensity : public BaseAction
	{
	  public:
		ChangeSSAOIntensity( Model::Renderer::RenderEffectPreset & p_preset, const int p_intensity ) :
			_preset( p_preset ), _intensity( p_intensity ) {};

		virtual void execute() override
		{
			_preset.setSSAOIntensity( _intensity );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _intensity;
	};
	class ChangeSSAOBlurSize : public BaseAction
	{
	  public:
		ChangeSSAOBlurSize( Model::Renderer::RenderEffectPreset & p_preset, const int p_blurSize ) :
			_preset( p_preset ), _blurSize( p_blurSize ) {};

		virtual void execute() override
		{
			_preset.setSSAOBlurSize( _blurSize );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _blurSize;
	};

	class EnableOutline : public BaseAction
	{
	  public:
		EnableOutline( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableOutline( _enable );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeOutlineThickness : public BaseAction
	{
	  public:
		ChangeOutlineThickness( Model::Renderer::RenderEffectPreset & p_preset, const float p_thickness ) :
			_preset( p_preset ), _thickness( p_thickness ) {};

		virtual void execute() override
		{
			_preset.setOutlineThickness( _thickness );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _thickness;
	};
	class ChangeOutlineColor : public BaseAction
	{
	  public:
		ChangeOutlineColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setOutlineColor( _color );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class EnableFog : public BaseAction
	{
	  public:
		EnableFog( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableFog( _enable );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeFogNear : public BaseAction
	{
	  public:
		ChangeFogNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override
		{
			_preset.setFogNear( _near );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeFogFar : public BaseAction
	{
	  public:
		ChangeFogFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override
		{
			_preset.setFogFar( _far );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};
	class ChangeFogDensity : public BaseAction
	{
	  public:
		ChangeFogDensity( Model::Renderer::RenderEffectPreset & p_preset, const float p_density ) :
			_preset( p_preset ), _density( p_density ) {};

		virtual void execute() override
		{
			_preset.setFogDensity( _density );
			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _density;
	};
	class ChangeFogColor : public BaseAction
	{
	  public:
		ChangeFogColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setFogColor( _color );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class ChangeBackgroundColor : public BaseAction
	{
	  public:
		ChangeBackgroundColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setBackgroundColor( _color );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};
	class ChangeCameraLightColor : public BaseAction
	{
	  public:
		ChangeCameraLightColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setCameraLightColor( _color );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};
	class ChangeCameraFOV : public BaseAction
	{
	  public:
		ChangeCameraFOV( Model::Renderer::RenderEffectPreset & p_preset, const float p_fov ) :
			_preset( p_preset ), _fov( p_fov ) {};

		virtual void execute() override
		{
			_preset.setCameraFOV( _fov );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _fov;
	};
	class ChangeCameraNear : public BaseAction
	{
	  public:
		ChangeCameraNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override
		{
			_preset.setCameraNearClip( _near );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeCameraFar : public BaseAction
	{
	  public:
		ChangeCameraFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override
		{
			_preset.setCameraFarClip( _far );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};

	class EnableCameraAntialiasing : public BaseAction
	{
	  public:
		EnableCameraAntialiasing( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.setAA( _enable );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeCameraPerspective : public BaseAction
	{
	  public:
		ChangeCameraPerspective( Model::Renderer::RenderEffectPreset & p_preset, const bool p_perspective ) :
			_preset( p_preset ), _perspective( p_perspective ) {};

		virtual void execute() override
		{
			_preset.setPerspectiveProjection( _perspective );

			if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
			{
				_preset.apply();
				VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
			}
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _perspective;
	};

	class AddNewPresetInLibrary : public BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_presetName ) : _presetName( p_presetName ) {}

		void execute()
		{
			Model::Renderer::RenderEffectPreset * const newRenderEffect
				= MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPreset>();

			newRenderEffect->setName( _presetName );
			Model::Renderer::RenderEffectPresetLibrary::get().addPreset( newRenderEffect );
		};

		const std::string _presetName;
	};

	class CopyPresetInLibrary : public BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		void execute() { Model::Renderer::RenderEffectPresetLibrary::get().copyPreset( _presetIndex ); };

	  private:
		const int _presetIndex;
	};

	class DeletePresetInLibrary : public BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		void execute()
		{
			Model::Renderer::RenderEffectPresetLibrary::get().deletePreset( _presetIndex );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const int _presetIndex;
	};
} // namespace VTX::Action::Renderer
#endif
