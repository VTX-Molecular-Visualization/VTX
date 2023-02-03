#ifndef __VTX_ACTION_RENDERER__
#define __VTX_ACTION_RENDERER__

#include "base_action.hpp"
#include "color/rgb.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Renderer
{
	class ReloadPresets : public BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override;
	};

	class ResetPresets : public BaseAction
	{
	  public:
		ResetPresets() {};
		virtual void execute() override;
	};

	class SavePreset : public BaseAction
	{
	  public:
		SavePreset( const Model::Renderer::RenderEffectPreset & p_preset )
		{
			_renderEffectPresets.emplace( &p_preset );
		};
		SavePreset( const std::unordered_set<const Model::Renderer::RenderEffectPreset *> & p_presets )
		{
			for ( const Model::Renderer::RenderEffectPreset * const preset : p_presets )
				_renderEffectPresets.emplace( preset );
		};
		SavePreset( Model::Renderer::RenderEffectPresetLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getPresetCount(); i++ )
				_renderEffectPresets.emplace( p_library.getPreset( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override;

	  private:
		std::unordered_set<const Model::Renderer::RenderEffectPreset *> _renderEffectPresets
			= std::unordered_set<const Model::Renderer::RenderEffectPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ApplyRenderEffectPreset : public BaseAction
	{
	  public:
		ApplyRenderEffectPreset( Model::Renderer::RenderEffectPreset & p_preset, const bool p_setAsDefault = false ) :
			_preset( p_preset ), _setAsDefault( p_setAsDefault ) {};
		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _setAsDefault;
	};

	class ChangeName : public BaseAction
	{
	  public:
		ChangeName( Model::Renderer::RenderEffectPreset & p_preset, const std::string & p_name ) :
			_preset( p_preset ), _name( Model::Renderer::RenderEffectPresetLibrary::get().getValidName( p_name ) ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const std::string					  _name;
	};

	class ChangeQuickAccess : public BaseAction
	{
	  public:
		ChangeQuickAccess( Model::Renderer::RenderEffectPreset & p_preset, const bool p_quickAccess ) :
			_preset( p_preset ), _quickAccess( p_quickAccess ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _quickAccess;
	};

	class ChangeShading : public BaseAction
	{
	  public:
		ChangeShading( Model::Renderer::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public BaseAction
	{
	  public:
		EnableSSAO( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeSSAOIntensity : public BaseAction
	{
	  public:
		ChangeSSAOIntensity( Model::Renderer::RenderEffectPreset & p_preset, const int p_intensity ) :
			_preset( p_preset ), _intensity( p_intensity ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _intensity;
	};
	class ChangeSSAOBlurSize : public BaseAction
	{
	  public:
		ChangeSSAOBlurSize( Model::Renderer::RenderEffectPreset & p_preset, const int p_blurSize ) :
			_preset( p_preset ), _blurSize( p_blurSize ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _blurSize;
	};

	class EnableOutline : public BaseAction
	{
	  public:
		EnableOutline( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeOutlineThickness : public BaseAction
	{
	  public:
		ChangeOutlineThickness( Model::Renderer::RenderEffectPreset & p_preset, const uint p_thickness ) :
			_preset( p_preset ), _thickness( p_thickness ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const uint							  _thickness;
	};
	class ChangeOutlineSensivity : public BaseAction
	{
	  public:
		ChangeOutlineSensivity( Model::Renderer::RenderEffectPreset & p_preset, const float p_sensivity ) :
			_preset( p_preset ), _sensivity( p_sensivity ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _sensivity;
	};
	class ChangeOutlineColor : public BaseAction
	{
	  public:
		ChangeOutlineColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class EnableFog : public BaseAction
	{
	  public:
		EnableFog( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeFogNear : public BaseAction
	{
	  public:
		ChangeFogNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeFogFar : public BaseAction
	{
	  public:
		ChangeFogFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};
	class ChangeFogDensity : public BaseAction
	{
	  public:
		ChangeFogDensity( Model::Renderer::RenderEffectPreset & p_preset, const float p_density ) :
			_preset( p_preset ), _density( p_density ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _density;
	};
	class ChangeFogColor : public BaseAction
	{
	  public:
		ChangeFogColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class ChangeBackgroundColor : public BaseAction
	{
	  public:
		ChangeBackgroundColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class ChangeCameraLightColor : public BaseAction
	{
	  public:
		ChangeCameraLightColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};
	class ChangeCameraFOV : public BaseAction
	{
	  public:
		ChangeCameraFOV( Model::Renderer::RenderEffectPreset & p_preset, const float p_fov ) :
			_preset( p_preset ), _fov( p_fov ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _fov;
	};
	class ChangeCameraNear : public BaseAction
	{
	  public:
		ChangeCameraNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeCameraFar : public BaseAction
	{
	  public:
		ChangeCameraFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};

	class EnableCameraAntialiasing : public BaseAction
	{
	  public:
		EnableCameraAntialiasing( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeCameraPerspective : public BaseAction
	{
	  public:
		ChangeCameraPerspective( Model::Renderer::RenderEffectPreset & p_preset, const bool p_perspective ) :
			_preset( p_preset ), _perspective( p_perspective ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _perspective;
	};

	class AddNewPresetInLibrary : public BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_presetName ) : _presetName( p_presetName ) {}

		virtual void execute() override;

		const std::string _presetName;
	};

	class CopyPresetInLibrary : public BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};

	class DeletePresetInLibrary : public BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};
} // namespace VTX::Action::Renderer
#endif
